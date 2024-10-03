# Spirograph Simulation Project Summary

## Technology Choices

1. **Application Framework**: Qt
   - Cross-platform
   - Good performance
   - Excellent for desktop applications with complex graphical requirements
   - Strong OpenGL support

2. **Programming Languages**:
   - C++ for core algorithms and performance-critical parts
   - Python for higher-level logic and UI (using PyQt)

3. **Graphics Library**: OpenGL (supported by Qt)

4. **Vector Mathematics**: Eigen (C++ template library for linear algebra)

5. **Version Control**: Git

6. **Build System**: CMake

7. **Testing Framework**:
   - Google Test for C++ unit testing
   - pytest for Python unit testing
   - Selenium for UI and integration testing

8. **Documentation**:
   - Doxygen for inline code documentation
   - Sphinx for generating user and technical documentation

9. **Continuous Integration**: GitHub Actions or GitLab CI

10. **Gcode Generation**: Custom C++ implementation, potentially using Clipper library for path operations

11. **File Formats**:
    - JSON for project files
    - SVG for vector export (using Cairo library)
    - PNG/JPEG export using Qt's image handling

12. **UI Design**: Qt Designer with custom styling using Qt Style Sheets (QSS)

## Qt Designer Overview

Qt Designer is a visual design tool for creating graphical user interfaces (GUIs) for Qt applications. Key features include:

- Drag-and-drop interface for adding and arranging UI elements
- Visual editing of widget properties
- Layout management tools for responsive designs
- Preview functionality
- Form inheritance and custom widget promotion
- Signals and slots connection editor

Qt Designer saves designs as .ui files (XML format), which can be loaded directly in Qt/PyQt applications or converted to Python code.

## Save and Load Functionality

### File Format (JSON)

```json
{
  "version": "1.0",
  "spirographParameters": {
    "outerGearRadius": 100,
    "innerGearRadius": 60,
    "penHoleDistance": 25,
    "rotations": 5
  },
  "customGearShape": {
    "type": "circular"
  },
  "patternType": "epitrochoid",
  "penHoles": [
    {"x": 20, "y": 0},
    {"x": 0, "y": 25}
  ],
  "visualSettings": {
    "strokeColor": "#FF0000",
    "strokeWidth": 2,
    "backgroundColor": "#FFFFFF"
  },
  "gcodeSettings": {
    "drawingAreaWidth": 200,
    "drawingAreaHeight": 200,
    "maxSpeed": 3000,
    "maxAcceleration": 500,
    "penUpCommand": "G0 Z5",
    "penDownCommand": "G0 Z0"
  }
}
```

### Python Implementation

```python
import json
from dataclasses import dataclass, asdict
from typing import List, Dict, Any

@dataclass
class SpirographDesign:
    version: str
    spirographParameters: Dict[str, float]
    customGearShape: Dict[str, Any]
    patternType: str
    penHoles: List[Dict[str, float]]
    visualSettings: Dict[str, Any]
    gcodeSettings: Dict[str, Any]

def save_design(design: SpirographDesign, filename: str):
    """Save the spirograph design to a JSON file."""
    with open(filename, 'w') as f:
        json.dump(asdict(design), f, indent=2)

def load_design(filename: str) -> SpirographDesign:
    """Load a spirograph design from a JSON file."""
    with open(filename, 'r') as f:
        data = json.load(f)
    return SpirographDesign(**data)

# Example usage:
design = SpirographDesign(
    version="1.0",
    spirographParameters={
        "outerGearRadius": 100,
        "innerGearRadius": 60,
        "penHoleDistance": 25,
        "rotations": 5
    },
    customGearShape={"type": "circular"},
    patternType="epitrochoid",
    penHoles=[{"x": 20, "y": 0}, {"x": 0, "y": 25}],
    visualSettings={
        "strokeColor": "#FF0000",
        "strokeWidth": 2,
        "backgroundColor": "#FFFFFF"
    },
    gcodeSettings={
        "drawingAreaWidth": 200,
        "drawingAreaHeight": 200,
        "maxSpeed": 3000,
        "maxAcceleration": 500,
        "penUpCommand": "G0 Z5",
        "penDownCommand": "G0 Z0"
    }
)

# Save the design
save_design(design, "my_spirograph_design.json")

# Load the design
loaded_design = load_design("my_spirograph_design.json")
print(loaded_design)
```

### Integration with Qt Application

1. Create menu items or buttons for "Save Design" and "Load Design".
2. Use QFileDialog for file selection when saving or loading.
3. Update UI elements with loaded values after loading a design.

This implementation provides a solid foundation for saving and loading designs, with good extensibility and human-readable file format.
