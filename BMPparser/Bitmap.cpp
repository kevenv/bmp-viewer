#include "Bitmap.h"

#include <sstream>

std::string BITMAP_FILE_HEADER::toString()
{
	std::stringstream ss;
	ss << "BITMAP_FILE_HEADER\n";
	ss << "signature= " << std::hex << signature << "\n";
	ss << "fileSize= " << fileSize << " bytes\n";
	ss << "reserved1= " << std::hex << reserved1 << "\n";
	ss << "reserved2= " << std::hex << reserved2 << "\n";
	ss << "pixelsOffset= " << std::hex << pixelsOffset;
	return ss.str();
}

std::string BITMAP_CORE_HEADER::toString()
{
	std::stringstream ss;
	ss << "BITMAP_CORE_HEADER\n";
	ss << "size= " << size << " bytes\n";
	ss << "width= " << width << " px\n";
	ss << "height= " << height << " px\n";
	ss << "planes= " << planes << "\n";
	ss << "bpp= " << bpp << " bits";
	return ss.str();
}

std::string BITMAP_INFO_HEADER::toString()
{
	std::stringstream ss;
	ss << "BITMAP_CORE_HEADER\n";
	ss << "size= " << size << " bytes\n";
	ss << "width= " << width << " px\n";
	ss << "height= " << height << " px\n";
	ss << "planes= " << planes << "\n";
	ss << "bpp= " << bpp << " bits\n";

	ss << "compression= " << compression << "\n";
	ss << "imageSize= " << imageSize << " bytes\n";
	ss << "xPxPerMeter= " << xPxPerMeter << "\n";
	ss << "yPxPerMeter= " << yPxPerMeter << "\n";
	ss << "usedColors= " << usedColors << "\n";
	ss << "importantColors= " << importantColors;
	return ss.str();
}

Bitmap::Bitmap(const std::string& filePath)
{
	load(filePath);
}

Bitmap::~Bitmap()
{
	if (isLoaded()) {
		delete[] pixels;
	}
}

void Bitmap::load(const std::string& filePath)
{
	file.open(filePath);
	if (file.isOpen()) {
		parseFileHeader();
		uint32 headerSize = file.read32();
		switch (headerSize) {
			/*case 0x10: //Core header
			{
			BITMAP_CORE_HEADER infoHeader;
			infoHeader.size = headerSize;
			infoHeader.width = file.read32();
			infoHeader.height = file.read32();
			infoHeader.planes = file.read16();
			infoHeader.bpp = file.read16();

			std::string str = infoHeader.toString();
			printf("%s\n", str.c_str());
			}
			break;*/
		case 0x28: //Info header
			parseInfoHeader(headerSize);
			parsePixels();
			break;
		default:
			printf("unrecognized header (%X)\n", headerSize);
			break;
		}
	}
}

bool Bitmap::isLoaded()
{
	return file.isOpen();
}

BITMAP_FILE_HEADER& Bitmap::getFileHeader()
{
	return fileHeader;
}

BITMAP_INFO_HEADER& Bitmap::getInfoHeader()
{
	return infoHeader;
}

uint8* Bitmap::getPixels()
{
	return pixels;
}

void Bitmap::toString()
{
	printf("%s\n", fileHeader.toString().c_str());
	printf("%s\n", infoHeader.toString().c_str());
	/*for (uint32 y = 0; y < infoHeader.height; y++) {
	for (uint32 x = 0; x < infoHeader.width; x++) {
	uint32 i = x*bytesPerPixel + y*infoHeader.width*bytesPerPixel;
	switch (infoHeader.bpp) {
	case 24:
	printf("(%X,%X,%X) ", pixels[i + 2], pixels[i + 1], pixels[i + 0]);
	break;
	default:
	printf("unimplemented bpp (%u)\n", infoHeader.bpp);
	}
	}
	printf("\n");
	}*/
}

void Bitmap::parseFileHeader()
{
	fileHeader.signature = file.read16();
	fileHeader.fileSize = file.read32();
	fileHeader.reserved1 = file.read16();
	fileHeader.reserved2 = file.read16();
	fileHeader.pixelsOffset = file.read32();
}

void Bitmap::parseInfoHeader(uint32 headerSize)
{
	infoHeader.size = headerSize;
	infoHeader.width = file.read32();
	infoHeader.height = file.read32();
	infoHeader.planes = file.read16();
	infoHeader.bpp = file.read16();

	infoHeader.compression = file.read32();
	infoHeader.imageSize = file.read32();
	infoHeader.xPxPerMeter = file.read32();
	infoHeader.yPxPerMeter = file.read32();
	infoHeader.usedColors = file.read32();
	infoHeader.importantColors = file.read32();
}

void Bitmap::parsePixels()
{
	uint32 paddingSize = infoHeader.width % 4;
	uint32 bytesPerPixel = infoHeader.bpp / 8;
	uint32 arraySize = infoHeader.imageSize - paddingSize*infoHeader.height;
	pixels = new uint8[arraySize];
	for (uint32 y = 0; y < infoHeader.height; y++) {
		for (uint32 x = 0; x < infoHeader.width*bytesPerPixel; x++) {
			uint8 tmp = file.read8();
			pixels[x + y*infoHeader.width*bytesPerPixel] = tmp;
			/*printf("%X\n", tmp);
			c++;*/
		}
		for (uint32 i = 0; i < paddingSize; i++) {
			uint8 tmp = file.read8();
			//printf("~%x\n", tmp);
		}
	}
}