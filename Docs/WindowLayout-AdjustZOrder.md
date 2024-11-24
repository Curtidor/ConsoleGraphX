The `AdjustZOrder` function iteratively adjusts the **Z-order** of a list of windows based on the order specified in the `windows` vector. Below is a detailed step-by-step explanation of how and why each window ends up either **above** or **below** others during the iteration.

### Detailed Breakdown of `AdjustZOrder`

The `AdjustZOrder` function iteratively adjusts the **Z-order** of a list of windows based on the order specified in the `windows` vector. Below is a detailed step-by-step explanation of how and why each window ends up either **above** or **below** others during the iteration.

---

### Example Input Vector

```cpp
std::vector<WindowZOrder> windows = {    
	 { mainWindow->GetHWND(), 0 },    
	 { editor->GetHWND(), 1 },        
	 { loggerWindow->GetHWND(), 2 },      
	 { RTIPWindow->GetHWND(), 3 }    
 };
```

### Code Logic

```cpp

for (size_t i = 0; i < windows.size(); ++i) 
{    
	HWND hwndInsertAfter = (i == 0) ? HWND_TOPMOST : windows[i - 1].hwnd;
	      
	if (!SetWindowPos(windows[i].hwnd, hwndInsertAfter, 0, 0, 0, 0, SWP_NOMOVE SWP_NOSIZE)) 
	{        
		std::cerr << "Failed to adjust Z-order for window at index " << i << ". Error: " << GetLastError() << "\n";
		 return false;     
	} 
}
```

## Iteration Breakdown and Z-order Explanation

#### **Initial State**

Before the loop begins:

- `windows` is sorted by `zOrder` in ascending order:
    `MAIN -> EDITOR -> LOGGER -> RTIP`
#### **First Iteration (`i = 0`)**

- **Window Being Processed**: `MAIN` (zOrder = 0)
- **`hwndInsertAfter`**: `HWND_TOPMOST`
- **Action**:
    - `SetWindowPos` places `MAIN` at the top of the Z-order stack as a **topmost** window.
- **Why?**:
    - The `HWND_TOPMOST` parameter ensures that `MAIN` is above all other windows, both topmost and non-topmost.
- **Stack After Iteration**:
```css
MAIN (Topmost)
```
---

#### **Second Iteration (`i = 1`)**

- **Window Being Processed**: `EDITOR` (zOrder = 1)
- **`hwndInsertAfter`**: `MAIN` (from `windows[i - 1]`)
- **Action**:
    - `SetWindowPos` places `EDITOR` relative to `MAIN`.
    - Since `MAIN` is topmost, `EDITOR` is placed **below** it in the Z-order.
- **Why?**:
    - Topmost windows (like `MAIN`) always stay above non-topmost windows (like `EDITOR`). The Z-order rules enforce this hierarchy.
- **Stack After Iteration**:
```css
MAIN (Topmost) 
EDITOR
   ``` 

---

#### **Third Iteration (`i = 2`)**

- **Window Being Processed**: `LOGGER` (zOrder = 2)
- **`hwndInsertAfter`**: `EDITOR` (from `windows[i - 1]`)
- **Action**:
    - `SetWindowPos` places `LOGGER` relative to `EDITOR`.
    - Since `EDITOR` is not topmost, `LOGGER` is placed **above** `EDITOR`.
- **Why?**:
    - Non-topmost windows stack naturally above earlier non-topmost windows in the order they are processed. Since `EDITOR` is not topmost, `LOGGER` can be placed above it.
- **Stack After Iteration**:
```css
MAIN (Topmost) 
LOGGER
EDITOR
    ```

---

#### **Fourth Iteration (`i = 3`)**

- **Window Being Processed**: `RTIP` (zOrder = 3)
- **`hwndInsertAfter`**: `LOGGER` (from `windows[i - 1]`)
- **Action**:
    - `SetWindowPos` places `RTIP` relative to `LOGGER`.
    - Since `LOGGER` is not topmost, `RTIP` is placed **above** `LOGGER`.
- **Why?**:
    - Similar to `LOGGER`, `RTIP` is a non-topmost window and is processed later. It is placed above `LOGGER` and remains above `EDITOR` as well.
- **Stack After Iteration**:
```css
MAIN (Topmost) 
RTIP 
LOGGER 
EDITOR
```

---

### Why This Behavior Happens

1. **Topmost vs. Non-Topmost Rules**:
    
    - The **main window** (`MAIN`) is topmost, so it always stays at the top of the Z-order stack.
    - The other windows (`EDITOR`, `LOGGER`, `RTIP`) are non-topmost and stack relative to each other.
2. **Relative Placement of Non-Topmost Windows**:
    
    - `LOGGER` and `RTIP` are placed **above** `EDITOR` because they are processed after `EDITOR` in the loop.
    - Since `EDITOR` is not topmost, Windows allows subsequent non-topmost windows (`LOGGER` and `RTIP`) to stack above it.
3. **Iterative Placement with `SetWindowPos`**:
    
    - `SetWindowPos` ensures each window is placed relative to the `hwndInsertAfter` handle.
    - This handle updates with each iteration, causing the observed stacking order.

---

### Final Z-Order Stack

After all iterations, the final Z-order stack is:


```css
MAIN (Topmost) 
RTIP 
LOGGER 
EDITOR
```

## General Rule of Thumb for Stacking Windows
* Start with the **topmost window first**, followed by **non-topmost windows in reverse priority order** (from lowest to highest Z-order). This ensures the stack behaves as expected without unexpected overlaps or reordering.