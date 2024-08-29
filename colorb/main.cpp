#include <Windows.h>
#include <chrono>
#include <string>
#include <sstream>
#include <stdexcept>
#include <cstdint>
#include <iostream>


/*
 * uInt Class
 * -----------------
 * This class represents a custom integer type optimized for high-performance
 * scenarios, particularly within the ConsoleGrahpx game engine render.
 *
 * The purpose of this class is to provide a compact and efficient way to store
 * integer values that can be directly copied into a char16_t array. It utilizes
 * bitwise operations to manipulate the internal representation of the integer
 * value, allowing for fast increments and conversions to char16_t arrays.
 *
 * Note: This class is designed specifically for internal use within the
 * ConsoleGrahpx game engine render and is not intended for use outside of this
 * context.
 */
class uInt {

private:
    static constexpr uint64_t BASE_VALUE = 13511005043687472;

    uint64_t _m_value;
    size_t _m_intValue;

private:
    void Increment() {
        size_t index = N;
        _m_intValue++;

        while (index > 0) {
            --index;
            // calculate the factor to isolate the current digit
            uint64_t factor = static_cast<uint64_t>(1) << (16 * index);

            // create a mask to isolate the current digit
            uint64_t mask = static_cast<uint64_t>(0xF) << (16 * index);

            // extract the current digit from the value
            uint64_t digit = (_m_value & mask) >> (16 * index);

            // increment the digit by 1 and ensure it wraps around if it reaches 10
            digit = (digit + 1) % 10;

            _m_value = (_m_value & ~mask) | (digit << (16 * index));

            if (digit != 0) {
                break;
            }
        }
    }

public:
    static constexpr size_t N = 4;

public:
    uInt() : _m_value(BASE_VALUE), _m_intValue(0) {}

    uInt& operator++() {
        Increment();
        return *this;
    }

    bool operator>=(const int& other) { return _m_intValue >= other; }

    void ToChar(char16_t outArray[4]) const
    {
        std::memcpy(outArray, &_m_value, std::min<int>((sizeof(char16_t) * N), sizeof(outArray)));
    }

    void Reset()
    {
        _m_value = BASE_VALUE;
        _m_intValue = 0;
    }

    uint64_t* GetValuePointer() {
        return &_m_value;
    }


    static constexpr short MaxSize() { return 9999; }
};


struct RawBuffer
{
    // Escape code sizes (does not account for null terminator)
    static constexpr int CURSOR_SET_SIZE = 3;
    static constexpr int PIXEL_ARRAY_SIZE = 20;
    static constexpr int NAME_SET_SIZE = 5;

    char16_t* m_buffer;
    char16_t* m_bufferCompressed;
    const int m_width;  // Width in characters
    const int m_height; // Height in lines
    const int m_nameLength;

    // Physical dimensions of the buffer
    const int m_initialSequenceSize;
    const int m_bufferWidth;  // Width in characters including newline
    const int m_bufferHeight; // Height in lines
    const int m_bufferSize;

    // Constructor to initialize the buffer with given width and height
    RawBuffer(const std::u16string& name, int width, int height)
        : m_buffer(nullptr), m_bufferCompressed(nullptr),
        m_width(width),
        m_height(height),
        m_bufferWidth((width* PIXEL_ARRAY_SIZE) + 1),  // Add 1 for newline character
        m_bufferHeight(height),
        m_nameLength(name.size()),
        m_initialSequenceSize(m_nameLength + CURSOR_SET_SIZE + NAME_SET_SIZE),
        m_bufferSize((m_bufferWidth* m_bufferHeight) + m_initialSequenceSize)
    {

        m_buffer = new char16_t[m_bufferSize];
        m_bufferCompressed = new char16_t[m_bufferSize];

        if (m_bufferSize < m_initialSequenceSize)
        {
            throw std::runtime_error("buffer size too small: Expected at least " + std::to_string(m_initialSequenceSize) + ", got " + std::to_string(m_bufferSize));
        }

        const char16_t pixelData[] = u"\033[38;2;004;010;020m\u2588";
        const std::u16string initialSetupSequence = u"\033[H\033]0;" + name + u"\007";

        std::memcpy(m_buffer, initialSetupSequence.c_str(), m_initialSequenceSize * sizeof(char16_t));

        char16_t* rowBuffer = new char16_t[m_bufferWidth];

        for (int x = 0; x < m_width * PIXEL_ARRAY_SIZE; x += PIXEL_ARRAY_SIZE) {
            std::memcpy(rowBuffer + x, pixelData, PIXEL_ARRAY_SIZE * sizeof(char16_t));
        }

        rowBuffer[m_width * PIXEL_ARRAY_SIZE] = u'\n';

        size_t bufferIndex = m_initialSequenceSize;
        for (int y = 0; y < m_height; ++y) {
            std::memcpy(m_buffer + bufferIndex, rowBuffer, m_bufferWidth * sizeof(char16_t));
            bufferIndex += m_bufferWidth;
        }

        delete[] rowBuffer;

        // copy the initial sequence into the compressed buffer
        std::memcpy(m_bufferCompressed, m_buffer, m_initialSequenceSize * sizeof(char16_t));
    }

    ~RawBuffer() {
        delete[] m_buffer;
    }

    // Function to modify the color of a specific pixel
    void SetPixelColor(int x, int y, uint8_t r, uint8_t g, uint8_t b)
    {
        if (x >= 0 && x < m_width && y >= 0 && y < m_height)
        {
            int index = CURSOR_SET_SIZE + y * m_bufferWidth + x * PIXEL_ARRAY_SIZE + 2;
            m_buffer[index] = static_cast<char16_t>(r);
            m_buffer[index + 2] = static_cast<char16_t>(g);
            m_buffer[index + 4] = static_cast<char16_t>(b);
        }
    }

    void SetNameUINTC(uInt& unicodeValue)
    {
        constexpr int offset = CURSOR_SET_SIZE + NAME_SET_SIZE - 1;
        std::memcpy(m_bufferCompressed + offset, unicodeValue.GetValuePointer(), uInt::N * sizeof(char16_t));
    }

    void SetNameC(const std::u16string& name)
    {
        constexpr int offset = CURSOR_SET_SIZE + NAME_SET_SIZE - 1;
        std::memcpy(m_bufferCompressed + offset, name.c_str(), std::min<int>(m_nameLength, name.length()) * sizeof(char16_t));
    }

    void SetNameUINT(uInt& unicodeValue)
    {
        constexpr int offset = CURSOR_SET_SIZE + NAME_SET_SIZE - 1;
        std::memcpy(m_buffer + offset, unicodeValue.GetValuePointer(), uInt::N * sizeof(char16_t));
    }

    void SetName(const std::u16string& name)
    {
        constexpr int offset = CURSOR_SET_SIZE + NAME_SET_SIZE - 1;
        std::memcpy(m_buffer + offset, name.c_str(), std::min<int>(m_nameLength, name.length()) * sizeof(char16_t));
    }

    int CompressBuffer() const
    {
        constexpr short COLOR_LENGTH = 11;
        constexpr short OFFSET_TO_NEWLINE_CHAR = 2;
        constexpr short OFFSET_TO_COLOR = 7;
        constexpr short COLOR_SIZE_BYTES = COLOR_LENGTH * sizeof(char16_t);
        constexpr short PIXEL_ARRAY_SIZE_BTYES = PIXEL_ARRAY_SIZE * sizeof(char16_t);
        // we use 7 for the size to omit the string terminator
        constexpr short RL_SIZE = 7;

        char16_t runLengthIndicator[RL_SIZE + 1] = u"\033[0000b";

        size_t lastPixelColorIndex = m_initialSequenceSize + OFFSET_TO_COLOR;
        uInt runningLength;
        size_t compressedSize = m_initialSequenceSize;

        int newLineOffset = 0;
        int isEqual = 0;
        for (int i = m_initialSequenceSize + OFFSET_TO_COLOR; i < m_bufferSize - newLineOffset; i += PIXEL_ARRAY_SIZE)
        {
            ++runningLength;

            isEqual = std::memcmp(m_buffer + i + newLineOffset, m_buffer + lastPixelColorIndex, COLOR_SIZE_BYTES);
            // the last pixel color is not equal to the current pixel color
            if (isEqual != 0)
            {
            }

            if (m_buffer[i + COLOR_LENGTH + OFFSET_TO_NEWLINE_CHAR + newLineOffset] == '\n')
            {
                // copies the pixel data
                std::memcpy(m_bufferCompressed + compressedSize, m_buffer + lastPixelColorIndex - OFFSET_TO_COLOR, PIXEL_ARRAY_SIZE_BTYES);
                // offset the runLengthIndicator pointer by 2 to index the number section
                std::memcpy(runLengthIndicator + 2, runningLength.GetValuePointer(), uInt::N * sizeof(char16_t));
                // copies the run length data
                std::memcpy(m_bufferCompressed + compressedSize + PIXEL_ARRAY_SIZE, runLengthIndicator, RL_SIZE * sizeof(char16_t));

                // adding 1 for the new line
                compressedSize += PIXEL_ARRAY_SIZE + RL_SIZE + 1;

                // remove 1 for the new line
                m_bufferCompressed[compressedSize - 1] = '\n';

                newLineOffset++;

                runningLength.Reset();
            }
        }

        // copies the pixel data
        std::memcpy(m_bufferCompressed + compressedSize, m_buffer + lastPixelColorIndex - OFFSET_TO_COLOR, PIXEL_ARRAY_SIZE_BTYES);
        // copies the run length data
        std::memcpy(m_bufferCompressed + compressedSize + PIXEL_ARRAY_SIZE, runLengthIndicator, 7 * sizeof(char16_t));

        compressedSize += PIXEL_ARRAY_SIZE + 7;

        return compressedSize;
    }
};


int main()
{
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

    CONSOLE_FONT_INFOEX fontInfo{};
    fontInfo.cbSize = sizeof(fontInfo);
    GetCurrentConsoleFontEx(hConsole, FALSE, &fontInfo);
    fontInfo.dwFontSize.X = 6;
    fontInfo.dwFontSize.Y = 3;
    SetCurrentConsoleFontEx(hConsole, FALSE, &fontInfo);

    DWORD dwMode = 0;
    GetConsoleMode(hConsole, &dwMode);
    dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
    SetConsoleMode(hConsole, dwMode);


    const int width = 600;
    const int height = 70;

    RawBuffer buffer(u"hello", width, height);

    auto lastTime = std::chrono::high_resolution_clock::now();
    DWORD dwBytesWritten = 0;

    uInt frameCount{};
    std::chrono::duration<float> elapsed;
    std::chrono::steady_clock::time_point currentTime;
    while (true)
    {
        int compressedBufferSize = buffer.CompressBuffer();

        //\033[33b
        WriteConsoleW(hConsole, buffer.m_bufferCompressed, compressedBufferSize, &dwBytesWritten, NULL);
        //WriteConsoleW(hConsole, buffer.m_buffer, buffer.m_bufferSize, &dwBytesWritten, NULL);

        ++frameCount;

        // Calculate FPS every second
        currentTime = std::chrono::high_resolution_clock::now();
        elapsed = currentTime - lastTime;
        if (elapsed.count() >= 1.0f)
        {
            float fps = *frameCount.GetValuePointer() / elapsed.count();
            lastTime = currentTime;

            buffer.SetNameUINTC(frameCount);
            frameCount.Reset();

        }

        //std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    return 0;
}
