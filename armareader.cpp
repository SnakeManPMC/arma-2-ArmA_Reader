/*

I guess this is some sort of DEBUG wrp reader which displays tons of debug text data on the screen.

For example it can cause huge slowdowns etc when displaying 500,000+ objects.

WRP_Stats should be more user friendly and "finished" wrp statistics util.

*/

#include <QtCore/QCoreApplication>
#include "armareader.h"
#include <cstdio>
#include <cstdlib>

armaReader::armaReader(int argc, char *argv[])
{
    Open_Files(argc, argv);

    Read_Signature();

    // 8WVR, ArmA style
    if (strcmp (sig, "8WVR") == 0) Read_8WVR();

    // 4WVR, OFP style
    if (strcmp (sig, "4WVR") == 0) Read_4WVR();

    Close_Files();
}


void armaReader::Open_Files(int argc, char *argv[])
{
    if (argc < 2)
    {
        printf("Error in parameters, use:\narma_reader.exe WRP_FILE\n");
        exit(1);
    }

    map = fopen(argv[1], "rb");
    if (!map)
    {
        printf("error in %s\n", argv[1]);
        exit(1);
    }

    printf ("Opened: %s\n", argv[1]);

    text = fopen("arma_reader_output.txt", "wt");
    if (!text)
    {
        printf("error at arma_reader_output.txt\n");
        exit(1);
    }
    printf ("Opened arma_reader_output.txt file for writing.\n");

    fprintf(text, "WRP file: %s\n", argv[1]);
}


void armaReader::Read_Signature()
{
    fread(sig, 4, 1, map);
    sig[4] = 0;
    fprintf(text, "WRP Signature: %s\n", sig);
    printf("WRP Signature: %s\n", sig);
}


void armaReader::Read_8WVR()
{
    fread(&texturegrid, 4, 1, map);
    fprintf(text, "texture grid x: %d\n", texturegrid);
    printf("texture grid x: %d\n", texturegrid);
    fread(&texturegrid, 4, 1, map);
    fprintf(text, "texture grid z: %d\n", texturegrid);
    printf("texture grid z: %d\n", texturegrid);

    fread(&terraingrid, 4, 1, map);
    fprintf(text, "terrain grid x: %d\n", terraingrid);
    printf("terrain grid x: %d\n", terraingrid);
    fread(&terraingrid, 4, 1, map);
    fprintf(text, "terrain grid z: %d\n", terraingrid);
    printf("terrain grid z: %d\n", terraingrid);

    // cell size, but how many digits??
    fread(&cellsize, 4, 1, map);
    fprintf(text, "Cellsize: %f\n", cellsize);
    printf("Cellsize: %f\n", cellsize);

    Read_8WVR_Elevations();

    Read_8WVR_Materials();

    Read_8WVR_Objects();

    fprintf (text, "All fine, 8WVR file read, exiting. Have a nice day!\n");
    printf ("All fine, 8WVR file read, exiting. Have a nice day!\n");
}


void armaReader::Read_8WVR_Elevations()
{
    // reading elevations
    for (int i = 0; i < terraingrid * terraingrid; i++)
    {
        fread(&elevation, 4, 1, map);
//		fprintf(text, "Elevation: %f\n", elevation);
        //printf("Elevation: %f\n", elevation);
    }
}


void armaReader::Read_8WVR_Materials()
{
    // credits go to Mikero for helping me figure out the RVMAT parts
    // reading rvmat index
    for (int i = 0; i < texturegrid * texturegrid; i++)
    {
        fread(&materialindex, 2, 1, map);
//		fprintf(text, "MaterialIndex: %d\n", materialindex);
//		printf("materialindex: %d\n", materialindex);
    }

    // noofmaterials
    fread(&noofmaterials, 4, 1, map);
    fprintf(text, "noofmaterials: %ld\n", noofmaterials);
    printf("noofmaterials: %ld\n", noofmaterials);

    // reading the first NULL material...
    fread(&materiallenght, 4, 1, map);
    fprintf(text, "1st NULL materiallenght: %d\n", materiallenght);
    printf("1st NULL materiallenght: %d\n", materiallenght);

    //////////// now, insert  the following
    noofmaterials--; // remove that 1st one. from the count

    while (noofmaterials--) //read the rest (if any)
    {
        fread(&materiallenght, 4, 1, map);
        if (!materiallenght)
        {
            printf("%ld materiallenght has no count\n", noofmaterials);
            fprintf(text, "%ld materiallenght has no count\n", noofmaterials);
            exit(1);
        }
        fread(&buffer, materiallenght, 1, map);
        buffer[materiallenght]=0; // make it asciiz
        printf("%ld material: %s\n", noofmaterials, buffer);
        fprintf(text, "%ld material: %s\n", noofmaterials, buffer);
        fread(&materiallenght, 4, 1, map);
        if (materiallenght)
        {
            printf("%ld 2nd materiallenght should be zero\n", noofmaterials);
            fprintf(text, "%ld 2nd materiallenght should be zero\n", noofmaterials);
            exit(1);
        }
    }
}


void armaReader::Read_8WVR_Objects()
{
    // Start reading objects...

    fprintf (text, "Reading 3d objects...\n");
    printf ("Reading 3d objects...\n");

    for(;;)
    {
        fread(&dDir, sizeof(float), 3*4, map);
        fread(&dObjIndex, sizeof(long), 1, map);
        fread(&len, sizeof(long), 1, map);

        if (!len) break;// last object has no name associated with it, and the transfrom is garbage

        fread(dObjName,sizeof(char),len,map);
        dObjName[len] = 0; // asciiz
        fprintf(text, "dObjIndex: %ld, %s\n", dObjIndex, dObjName);
        // why was this here, huge spam? :)
        //printf("dObjIndex: %ld, %s\n", dObjIndex, dObjName);
    }
    // should now be at eof
}


void armaReader::Read_4WVR()
{
    fread(&texturegrid, 4, 1, map);
    printf("texture grid x: %d\n", texturegrid);
    fprintf(text, "texture grid x: %d\n", texturegrid);
    fread(&texturegrid, 4, 1, map);
    fprintf(text, "texture grid z: %d\n", texturegrid);
    printf("texture grid z: %d\n", texturegrid);

    Read_4WVR_Elevations();

    Read_4WVR_Textures();

    Read_4WVR_Objects();

    fprintf (text, "All fine, 4WVR file read, exiting. Have a nice day!\n");
    printf ("All fine, 4WVR file read, exiting. Have a nice day!\n");
}


void armaReader::Read_4WVR_Elevations()
{
    // reading elevations
    for (int i = 0; i < texturegrid * texturegrid; i++)
    {
        // this was elevation before (float), but it supposed to be short??
        fread(&materialindex, sizeof(short), 1, map);

        // the float thing ;)
        fprintf(text, "Elevation: %f\n", (materialindex / 22.222f) );
    }
}


void armaReader::Read_4WVR_Textures()
{
    printf("Reading texture indexes...");

    // read textures indexes
    for (int i = 0; i < texturegrid * texturegrid; i++)
    {
        fread(&materialindex, sizeof(short), 1, map);
    }

    printf(" Done\nReading texture names...");

    // textures 32 char length and total of 512
    for (int ix = 0; ix < 512; ix++)
    {
        sig[0] = 0;
        fread(sig, 32, 1, map);
        fprintf(text, "texture name [%d]: %s\n", ix, sig);
    }
}


void armaReader::Read_4WVR_Objects()
{
    printf(" Done\nReading 3d objects...\n");

    while (!feof(map))
    {
        fread(&dDir, sizeof(float), 3*4, map);
        fread(&dObjIndex, sizeof(long), 1, map);
        fread(dObjName, 76, 1, map);
        dObjName[76] = 0; // asciiz
        fprintf(text, "dObjIndex: %ld, %s\n", dObjIndex, dObjName);
        // why was this here, huge spam? :)
        //printf("dObjIndex: %ld, %s\n", dObjIndex, dObjName);
    }
    // should now be at eof
}


void armaReader::Close_Files()
{
    fclose(map);
    fclose(text);
}


/*
8WVR
{
    WRPHeader     Header
    float         Elevations   [TerrainGridSize.y][TerrainGridSize.x];
    ushort        MaterialIndex[TextureGridSize.y][TextureGridSize.x]; //zero based index into MaterialNames
    ulong         NoOfMaterials;
    MaterialName  MaterialNames[NoOfMaterials];
    Object        Objects[...];
}

WrpHeader
{
    char    Filetype;        // "8WVR"
    XYPair  TextureGridSize; // 256 x 256 eg
    XYPair  TerrainGridSize; // ditto
    float   CellSize;        // generally 50.0 meters
}

MaterialName
{
    Strings RvMatFileNames[...]; //[Length]:"SomePboPrefix\SomeIsland\data\layers\p_002-000_l02.rvmat" ... "\0x0000";
}

Object
{
    float  TransformMatrix[3][4]; // standard directX transform matrix
    ulong  ObjectId;
    long lenght;
    String P3dFileName[Length];   // "ca\buildings\ryb_domek.p3d"
}
*/

/*

MaterialName
{
    Strings RvMatFileNames[...]; //[Length]:"SomePboPrefix\SomeIsland\data\layers\p_002-000_l02.rvmat" ... "\0x0000";
}

Conceptually, these are lists of all the textures that are used by this 'world'.
The MaterialIndex array indicates what type of textures (plural) should be used
in any given cell. Ie any 50 meter chunk of the map. Thus, you should fully
expect to see a lot of the same index value for 'sea' texture .

Endemic to all wrp formats: BI use concatenated Strings to express a series of
rvmat files for the given cell. More than one rvmat file *could* be used per cell.
In practice this never happens. However, the construct exists in all Wrp formats
for it to be used.

In this wrp format, specifically 8WVR, BI use Length Strings rather than the more
familiar Asciiz strings. The end result is the same. A zero length entry, is,
the end of (this series of) concatenated rvmat files.

At best, there happens to be only one String per cell entry. That fact does not
negate the construct.

Length:Ascii:AnotherLength....

To complicate the matter slightly, A zero value in the MaterialIndex array would
indicate 'no rvmat' (no texture) for this cell. In practice, a zero never occurs
for any cell (all cells have a texture) but the construct is there for it to
happen. This too is endemic to all wrp formats.

To this end, the very first MaterialName entry is always null. There is no file
associated with it, because it will never be accessed. For the first entry only
then there is a single ulong length value (four bytes) == 0. All other entries
contain a length , followed by a string, followed by a length of zero. The
latter, meaning, no more rvmats for this cell.

    * Note that this list of rvmat files while intended to only contain unique
    instances, does not. Occasionally, the very same rvmat file is listed more
    than once (with an obviously different index entry to each).
*/

/*
Object
{
    float  TransformMatrix[3][4]; // standard directX transform matrix
    ulong  ObjectId;
    String P3dFileName[Length];   // "ca\buildings\ryb_domek.p3d"
}
*/

/*
Type 	Description

byte	unsigned 8 bit (1 byte)
char	signed 8 bit Ascii(utf8)character
char[]	fixed length string
tbool	byte (0 = false).
short	16 bit signed short (2 bytes)
ushort	16 bit unsigned short (2 bytes)
long	32 bit signed integer (4 bytes)
ulong	32 bit unsigned integer (4 bytes)
float	32 bit IEEE-single precision floating point value (4 bytes)
double	64 bit IEEE-double precision floating point value (8 bytes)
asciiz	Null terminated (0x00) variable length ascii string
asciiz...	zero or more concatenated asciiz strings
ascii	fixed length ascii string(UTF-8)

*/
