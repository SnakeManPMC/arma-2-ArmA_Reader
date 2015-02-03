#ifndef ARMAREADER_H
#define ARMAREADER_H

class armaReader
{
public:
    armaReader(int argc, char *argv[]);
    void Open_Files(int argc, char *argv[]);
    void Read_Signature();
    void Read_8WVR();
    void Read_4WVR();
    void Read_8WVR_Elevations();
    void Read_4WVR_Elevations();
    void Read_8WVR_Materials();
    void Read_4WVR_Textures();
    void Read_8WVR_Objects();
    void Read_4WVR_Objects();
    void Close_Files();

private:
    FILE *map;
    FILE *text;
    char buffer[2000];
    char dObjName[2000];
    char sig[33];
    float cellsize, elevation, dDir[3][4];
    int len, texturegrid, terraingrid;
    long noofmaterials;
    short materialindex, materiallenght;
    ulong dObjIndex;

};

#endif // ARMAREADER_H
