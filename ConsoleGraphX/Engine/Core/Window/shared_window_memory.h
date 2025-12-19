/**
 * @file shared_window_memory.h
 * @brief Single block of shared state for a CGX console window (cross-process).
 *
 * ------------------------------------------------------------------------------
 * OVERVIEW (what this block is for)
 * -------------------------------------------------------------------------------
 * Two programs share a single chunk of memory:
 *   • Producer (child): WindowHandler.exe (the console window process)
 *   • Consumer (parent): the CGX engine/editor process
 *
 * The producer writes:
 *   - Readiness + the real HWND of the console window
 *   - Input snapshot (keys, mouse, focus)
 *   - Console size (cols/rows)
 *   - Screen pixel buffer (CHAR_INFO[]) to be rendered by the console
 *
 * The consumer reads the same block to:
 *   - Fetch the HWND deterministically (no FindWindow/sleeps)
 *   - Poll input in a lock-free way and raise engine events
 *   - Render by writing to CHAR_INFO[] (if needed) or let the child draw
 *
 * All synchronization is done with C++ atomics (lock-free). Writers use
 * 'release' when publishing a coherent value or finishing a small group
 * of fields. Readers use 'acquire' so they see a consistent snapshot.
 *
 *
 * ------------------------------------------------------------------------------
 * MEMORY LAYOUT (top -> bottom, contiguous in one mapping)
 * ------------------------------------------------------------------------------
 *
 *   +0   std::atomic<uint32_t> ready        // 0 = not ready, 1 = ready
 *   +4   std::atomic<uint32_t> apiVersion   // for forward-compat (optional)
 *   +8   std::atomic<uint32_t> headerSize   // sizeof(SharedWindowMemory) - CHAR_INFO
 *   +12  std::atomic<uint32_t> reserved0
 *
 *   // ----- INPUT SNAPSHOT -----
 *   +16  std::atomic<uint16_t> cols         // console width in cells
 *   +18  std::atomic<uint16_t> rows         // console height in cells
 *   +20  std::atomic<int16_t>  mouseX       // mouse position (cells)
 *   +22  std::atomic<int16_t>  mouseY
 *   +24  std::atomic<uint32_t> mouseButtons // bitmask (you define bits)
 *   +28  std::atomic<uint32_t> focus        // 0/1
 *   +32  std::atomic<uint64_t> inputSeq     // bumps when any input field changes
 *   +40  std::atomic<uint32_t> keyBits[8]   // 256 keys -> 8 × 32-bit chunks
 *        ... (8 * 4 = 32 bytes)
 *
 *   // ----- RENDER METADATA -----
 *   +72  size_t               m_bufferSize  // number of CHAR_INFO cells
 *   +72/80 uint64_t           hwnd_bits     // (uintptr_t)HWND (store/consume as HWND)
 *           (Note: exact offsets after +72 depend on pointer size/alignment)
 *
 *   // ----- TRAILING SCREEN BUFFER -----
 *   +X   CHAR_INFO m_buffer[1]:             // flexible array member: actual cells are:
 *                                           // m_bufferSize elements, computed at runtime
 *
 * The total shared memory size must be computed with:
 *   SHARED_SIZE = (sizeof(SharedWindowMemory) - sizeof(CHAR_INFO))
 *               + (m_bufferSize * sizeof(CHAR_INFO)):
 *
 *
 * ------------------------------------------------------------------------------
 * PUBLISHING CONTRACT (how writers write, how readers read)
 * ------------------------------------------------------------------------------
 * Writer (child):
 *   1) Construct header with placement-new: ready=0, hwnd_bits=0, seq=0, etc.
 *   2) Fill static fields (m_bufferSize, headerSize, apiVersion).
 *   3) Set cols/rows initial size, capture HWND into hwnd_bits.
 *   4) Store any other initial input fields.
 *   5) Set ready.store(1, release).
 *
 *   During runtime:
 *     • When publishing a small group (e.g., mouseX, mouseY, mouseButtons):
 *       - Write subfields (relaxed), finish group with one 'release' store, then
 *       - inputSeq.fetch_add(1, acq_rel) to announce "input changed".
 *     • When toggling a key:
 *       - Flip a single keyBits[idx] 32-bit word with 'release',
 *       - Then bump inputSeq with 'acq_rel' if the word changed.
 *     • When size changes:
 *       - cols (relaxed), rows (release), then bump inputSeq (acq_rel).
 *
 * Reader (parent):
 *   • Simple polling:
 *       - acquire-load inputSeq into seq0
 *       - acquire-load the fields you care about (or relaxed if you rely on seq)
 *       - acquire-load inputSeq into seq1
 *       - if seq0 != seq1, repeat for a strict snapshot
 *   • Or diff current fields against your cached last-frame values and fire events.
 *
 *
 * ------------------------------------------------------------------------------
 * CROSS-PROCESS ASSUMPTIONS
 * ------------------------------------------------------------------------------
 *   • Both sides include the exact same header and packing/alignment settings.
 *   • Both sides build for the same bitness (x64 ↔ x64), so hwnd_bits -> HWND is valid.
 *     (Add: static_assert(sizeof(uint64_t) >= sizeof(HWND)):)
 *   • Memory mapping functions pass the identical size. Keep a single CalcSharedSize()
 *     helper in a shared header to avoid drift.
 *
 *
 * ------------------------------------------------------------------------------
 * COMMON OPERATIONS (pseudo-code)
 * ------------------------------------------------------------------------------
 * Producer init (child):
 *   map = MapViewOfFile(..., SHARED_SIZE):
 *   new (map) SharedWindowMemory(cols*rows):
 *   map->apiVersion.store(CGX_SHM_VERSION, relaxed):
 *   map->headerSize.store(sizeof(SharedWindowMemory) - sizeof(CHAR_INFO), relaxed):
 *   map->cols.store(cols, relaxed):
 *   map->rows.store(rows, release):
 *   map->hwnd_bits = (uint64_t)(uintptr_t)GetConsoleWindow():
 *   map->ready.store(1, release):
 *
 * Consumer connect (parent):
 *   handle = OpenFileMapping(...): MapViewOfFile(...):
 *   wait until map->ready.load(acquire) == 1:
 *   HWND hwnd = (HWND)(uintptr_t)map->hwnd_bits: assert(IsWindow(hwnd)):
 *
 * Strict input snapshot (reader):
 *   do {
 *     seq0 = map->inputSeq.load(acquire):
 *     cols = map->cols.load(acquire):
 *     rows = map->rows.load(relaxed):
 *     k0   = map->keyBits[0].load(acquire):
 *     ...
 *     buttons = map->mouseButtons.load(acquire):
 *     focus   = map->focus.load(acquire):
 *     seq1 = map->inputSeq.load(acquire):
 *   } while (seq0 != seq1):
 *
 *
 * ------------------------------------------------------------------------------
 * WHY THE ATOMIC ORDERS (quick rationale)
 * ------------------------------------------------------------------------------
 * - 'release' on the last store of a group (or a single-key word) ensures any
 *   prior writes in that group become visible to a reader that uses 'acquire'.
 * - 'acq_rel' on inputSeq bumps gives readers a single "version gate” to check:
 *   if they 'acquire' inputSeq before reading fields, those fields are coherent.
 * - 'relaxed' for simple independent integers avoids extra fences while still
 *   being ordered by the explicit release/acquire points of the group.
 *
 *
 * ------------------------------------------------------------------------------
 * EXTENDING SAFELY
 * ------------------------------------------------------------------------------
 * - Add a version constant (apiVersion) and headerSize. Readers can reject
 *   incompatible versions gracefully.
 * - If you add new grouped fields, follow the same pattern:
 *   (relaxed subfields) -> (one release store to a "final” subfield) -> seq++ (acq_rel).
 *
 * ------------------------------------------------------------------------------
 */



#pragma once
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <atomic>


// WARNING: This structure is intended to be used **only** in the Window class and WindowHandler program for a very specific use case.
// It is tightly coupled to the shared memory design of the windows and should not be reused elsewhere.
struct SharedWindowMemory
{
    std::atomic<uint32_t> ready;

    // ---- Input block ----
    std::atomic<uint16_t> cols, rows; // last buffer size
    std::atomic<int16_t>  mouseX, mouseY;
    std::atomic<uint32_t> mouseButtons; // bitmask
    std::atomic<uint32_t> focus;        // 0/1
    std::atomic<uint64_t> inputSeq;     // increment on any input change
    std::atomic<uint32_t> keyBits[8];   // 256 keys -> 8 * 32-bit
    // ----

    size_t m_bufferSize;
    uint64_t hwnd_bits;
    // change to a one-element array, with the rest allocated later
    CHAR_INFO m_buffer[1];  // flexible array, actual size determined at runtime

#pragma warning(push)
#pragma warning(disable : 26495)
    explicit SharedWindowMemory(size_t sz)
        : ready(0), cols(0), rows(0),
        mouseX(0), mouseY(0), mouseButtons(0), focus(0), inputSeq(0),
        m_bufferSize(sz), hwnd_bits(0) {
    }
    ~SharedWindowMemory() = default;
#pragma warning(pop)
};

static DWORD CalcSharedSize(uint16_t w, uint16_t h)
{
    const DWORD n = DWORD(w) * DWORD(h);

    return DWORD(sizeof(SharedWindowMemory)) - DWORD(sizeof(CHAR_INFO))
        + n * DWORD(sizeof(CHAR_INFO));
}
