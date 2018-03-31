#ifndef TYPES_H
#define TYPES_H

enum types
{
    Cube1x1, Cube1x2, Cube1x3, Cube1x4, Cube1x6, Cube1x8, Cube1x10,
    Cube2x2, Cube2x3, Cube2x4, Cube2x6, Cube2x8, Cube2x10,
    Cube4x4, Cube4x6, Cube4x8, Cube4x10,
    Cube8x8,
    Plate1x1, Plate1x2, Plate1x3, Plate1x4, Plate1x6, Plate1x8, Plate1x10,
    Plate2x2, Plate2x3, Plate2x4, Plate2x6, Plate2x8, Plate2x10,
    Plate4x4, Plate4x6, Plate4x8, Plate4x10,
    Plate8x8,
    Arc1x3, Arc1x4, Arc1x6,
    Angle1x2, Angle1x3,
    Angle2x2, Angle2x3,
    Anglev2x2, Anglev2x3,
    Base24x24    
};

struct Identify
{
    Identify() { }
    Identify(int idDetail, int w, int l)
        : id(idDetail), sp_width(w), sp_length(l), sp_height(3) {  }
    Identify(int idDetail, int w, int l, int h)
        : id(idDetail), sp_width(w), sp_length(l), sp_height(h) {  }
    int id;
    int sp_width;
    int sp_length;
    int sp_height;
    // Значения для sp_height
    // 0 - базовая плата;
    // 1 - пластина;
    // 3 - стандартный размер (высота обычного куба);
};

#endif // TYPES_H
