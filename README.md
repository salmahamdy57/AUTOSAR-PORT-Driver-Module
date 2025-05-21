# AUTOSAR-PORT-Driver-Module

### Project Overview 📖
Develop an **AUTOSAR-compliant** PORT driver module for a microcontroller platform, providing standardized access and configuration of I/O ports and pins. The driver adheres to the AUTOSAR PORT specification, enabling safe, portable, and configurable pin initialization, direction control, and mode management.

### Objectives 🎯
- Implement the AUTOSAR **PORT** driver API as defined in the SWS document
- Enable static and dynamic pin configuration (direction, mode, level)
- Support versioning, development error reporting (DET), and compliance with **MISRA‑C 2004** rules
- Provide robust abstraction over hardware registers for portability across ECUs

### Features 💡
- **Initialize Ports**: `Port_Init(const Port_ConfigType* ConfigPtr)` to set pin directions and modes at startup
- **Set Pin Direction**: `Port_SetPinDirection(Port_PinType Pin, Port_PinDirectionType Direction)` for runtime changes (if allowed)
- **Refresh Direction**: `Port_RefreshPortDirection()` to reapply static directions
- **Set Pin Mode**: `Port_SetPinMode(Port_PinType Pin, Port_PinModeType Mode)` to switch between analog/digital or peripheral functions
- **Version Info**: `Port_GetVersionInfo(Std_VersionInfoType* versioninfo)` for module metadata
- **Error Reporting**: Integration with **Det_ReportError** for development-time checks (e.g. null pointers, invalid parameters)

### Drivers & API 📚
Conforms to **AUTOSAR 4.x** PORT SWS, with reentrancy and runtime parameter checks:

- **Initialization:**
  ```c
  void Port_Init(const Port_ConfigType* ConfigPtr);

- **Pin Control:**
  ```c
  void Port_SetPinDirection(Port_PinType Pin, Port_PinDirectionType Direction);
  void Port_RefreshPortDirection(void);
  void Port_SetPinMode(Port_PinType Pin, Port_PinModeType Mode);

- **Versioning and Errors:**
  ```c
  void Port_GetVersionInfo(Std_VersionInfoType* versioninfo);

- **Development Error Tracer (DET) Integration:**  
  Reports errors when APIs are called with invalid arguments or in wrong state  

### Requirements 📋
- Conform to AUTOSAR PORT SWS and MISRA‑C 2004 standards
- Use `Port_ConfigType` structures for pin setup in `Port_Cfg.c`
- Structure code into `src/`, `include/`, `config/`, and `tests/` directories
- Provide Doxygen-style comments for all APIs  

### Implementation Highlights 🚀
- Error Checking: `Det_ReportError` for null pointers and invalid parameters
- Register Unlocking: Special handling for protected pins (PD7, PF0)
- Pin Setup: Direction, resistor, and mode configured via register offsets
- Atomic Register Access: `SET_BIT`/`CLEAR_BIT` macros for safe bit operations
- Version API: Optional `Port_GetVersionInfo` for metadata retrieval
