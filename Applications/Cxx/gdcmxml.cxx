/*=========================================================================

  Program: GDCM (Grassroots DICOM). A DICOM library

  Copyright (c) 2006-2011 Mathieu Malaterre
  All rights reserved.
  See Copyright.txt or http://gdcm.sourceforge.net/Copyright.html for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#include "gdcmFilename.h"
#include "gdcmReader.h"
#include "gdcmVersion.h"
#include "gdcmFileMetaInformation.h"
#include "gdcmDataSet.h"
#include "gdcmDataElement.h"
#include "gdcmAttribute.h"
#include "gdcmPrivateTag.h"
#include "gdcmValidate.h"
#include "gdcmWriter.h"
#include "gdcmSystem.h"
#include "gdcmDirectory.h"
#include "gdcmCSAHeader.h"
#include "gdcmPDBHeader.h"
#include "gdcmSequenceOfItems.h"
#include "gdcmASN1.h"
#include "gdcmFile.h"
#include "gdcmXMLPrinter.h"

#ifdef GDCM_USE_SYSTEM_LIBXML2
#include <libxml/xmlreader.h>
#endif

#include <string>
#include <iostream>
#include <fstream>

#include <stdio.h>     /* for printf */
#include <stdlib.h>    /* for exit */
#include <getopt.h>
#include <string.h>

using namespace gdcm;

// This is a very dumb implementation for getData handling
// by default GDCM simply drop the BulkData so we need to store
// the actual BulkData somewhere for proper implementation of getData
class SimpleFileXMLPrinter : public XMLPrinter
{
public:
  void HandleBulkData(const char *uuid,
    const char *bulkdata, size_t bulklen)
    {
    std::ofstream out( uuid, std::ios::binary );
    out.write( bulkdata, bulklen );
    out.close();
    }
};

void PrintVersion()
{
  std::cout << "gdcmxml: gdcm " << gdcm::Version::GetVersion() << " ";
  const char date[] = "$Date$";
  std::cout << date << std::endl;
}

void PrintHelp()
{
  PrintVersion();
  std::cout << "Usage: gdcmxml [OPTION]... FILE..." << std::endl;
  std::cout << "Convert a DICOM file into an XML file or vice-versa \n";
  std::cout << "Parameter (required):" << std::endl;
  std::cout << "  -i --input     Filename1" << std::endl;
  std::cout << "  -o --output    Filename2" << std::endl;
  std::cout << "General Options:" << std::endl;
  std::cout << "  -B --loadBulkData   for DICOM -> XML, loads all bulk data like Pixel Data (by default UUID are written)." << std::endl;
  std::cout << "  -V --verbose        more verbose (warning+error)." << std::endl;
  std::cout << "  -W --warning        print warning info." << std::endl;
  std::cout << "  -D --debug          print debug info." << std::endl;
  std::cout << "  -E --error          print error info." << std::endl;
  std::cout << "  -h --help           print help." << std::endl;
  std::cout << "  -v --version        print version." << std::endl;
}

#ifdef GDCM_USE_SYSTEM_LIBXML2
static void CreateDataElement(xmlTextReaderPtr reader,const DataElement &de) 
{
  const xmlChar *name, *vr_read, *keyword, *tag_read, *privateowner, *value;
  name = xmlTextReaderConstName(reader);
  if (strcmp((const char*)name,"DicomAttribute") == 0)
  {
  vr_read = xmlTextReaderGetAttribute(reader,(const unsigned char*)"vr");
  tag_read = xmlTextReaderGetAttribute(reader,(const unsigned char*)"tag");
  Tag t;
  if(!t.ReadFromContinuousString((const char *)tag_read))
  	assert(0 && "Invalid Tag!");
  std::cout << t; 
  printf("hey\n");
  //vr = xmlTextReaderGetAttribute(reader,"privateowner");
  printf("hey 1) %s %s \n", vr_read, tag_read);
  if (name == NULL)
    name = BAD_CAST "--";
  /*
  value = xmlTextReaderConstValue(reader);
  printf("heyp %d %d %s %d %d", 
    xmlTextReaderDepth(reader),
    xmlTextReaderNodeType(reader),
    name,
    xmlTextReaderIsEmptyElement(reader),
    xmlTextReaderHasValue(reader));
      
  if(value == NULL)
    printf("\n");
  else 
    {
    if (xmlStrlen(value) > 40)
      printf(" %.40s...\n", value);
    else
      printf(" %s\n", value);
    }*/
   } 
}

static void PopulateDataSet(xmlTextReaderPtr reader,const DataSet &DS)
{
  //const DataSet *ds;
  const DataElement de;
	int ret = xmlTextReaderRead(reader);
	//ret = xmlTextReaderRead(reader);//move past starting tag
  while (ret == 1) 
    {
    //create Data Element with attributes
    
    CreateDataElement(reader,de);
    //put value - Handle SQ, UN, fragments etc.
    
    //Insert Into DataSet
    
    //Continue traversing
    ret = xmlTextReaderRead(reader);
    }
  xmlFreeTextReader(reader);
  if (ret != 0) 
    {
    fprintf(stderr, "Failed to parse XML file\n");
    exit(1);
    }
  
  //return ds;  
}

static void WriteDICOM(xmlTextReaderPtr reader, gdcm::Filename file2)
{
	//populate DS
  const DataSet DS;
  PopulateDataSet(reader,DS);
  //add to File 
  //File F;
  //F.SetDataSet(DS);
  
  //Validate - possibly from gdcmValidate Class
  
  //add to Writer
  Writer W;
  //W.SetFile(F);  
  //W.CheckFileMetaInformationOff();
  //W.SetFileName(file2.GetFileName());
  
  //finally write to file
  //W.Write(); 
  
}

static void XMLtoDICOM(gdcm::Filename file1, gdcm::Filename file2)
{
  xmlTextReaderPtr reader;  
  FILE *in;
  char *buffer;
  long numBytes;
  in = fopen(file1.GetFileName(), "r");
  
  if(in == NULL)
    return ;
    
  fseek(in, 0L, SEEK_END);
  numBytes = ftell(in);
  fseek(in, 0L, SEEK_SET);
  buffer = (char*)calloc(numBytes, sizeof(char));

  if(buffer == NULL)
    return ;

  fread(buffer, sizeof(char), numBytes, in);
  fclose(in);
  reader = xmlReaderForMemory  (buffer, numBytes, NULL, NULL, 0);
  //reader = xmlReaderForFile(filename, "UTF-8", 0);
  if (reader != NULL) 
    {
    WriteDICOM(reader, file2);
    } 
  else 
    {
    fprintf(stderr, "Unable to open %s\n", file1.GetFileName());
     }
}
#endif // GDCM_USE_SYSTEM_LIBXML2

int main (int argc, char *argv[])
{
  int c;
  //int digit_optind = 0;
  gdcm::Filename file1;
  gdcm::Filename file2;
  int loadBulkData = 0;
  int verbose = 0;
  int warning = 0;
  int debug = 0;
  int error = 0;
  int help = 0;
  int version = 0;
  while (1) {

    int option_index = 0;

    static struct option long_options[] = {
        {"input", 1, 0, 0},
        {"output", 1, 0, 0},
        {"loadBulkData", 0, &loadBulkData, 1},
        {"verbose", 0, &verbose, 1},
        {"warning", 0, &warning, 1},
        {"debug", 0, &debug, 1},
        {"error", 0, &error, 1},
        {"help", 0, &help, 1},
        {"version", 0, &version, 1},
        {0, 0, 0, 0} // required
    };
    static const char short_options[] = "i:o:BVWDEhv";
    c = getopt_long (argc, argv, short_options,
      long_options, &option_index);
    if (c == -1)
      {
      break;
      }

    switch (c)
      {
    case 0:
    case '-':
        {
        const char *s = long_options[option_index].name;
        if (optarg)
          {
          if( option_index == 0 ) /* input */
            {
            assert( strcmp(s, "input") == 0 );
            assert( file1.IsEmpty() );
            file1 = optarg;
            }
          }
        }
      break;

    case 'i':
      //printf ("option i with value '%s'\n", optarg);
      assert( file1.IsEmpty() );
      file1 = optarg;
      break;

    case 'o':
      assert( file2.IsEmpty() );
      file2 = optarg;
      break;

    case 'B':
      loadBulkData = 1;
      break;

    case 'V':
      verbose = 1;
      break;

    case 'W':
      warning = 1;
      break;

    case 'D':
      debug = 1;
      break;

    case 'E':
      error = 1;
      break;

    case 'h':
      help = 1;
      break;

    case 'v':
      version = 1;
      break;

    case '?':
      break;

    default:
      printf ("?? getopt returned character code 0%o ??\n", c);
      }
  }

  if (optind < argc)
    {
    int v = argc - optind;
    if( v == 2 )
      {
      file1 = argv[optind];
      file2 = argv[optind+1];
      }
    else if( v == 1 )
      {
      file1 = argv[optind];
      }
    else
      {
      PrintHelp();
      return 1;
      }
    }

  if( file1.IsEmpty() )
    {
    PrintHelp();
    return 1;
    }

  if( version )
    {
    PrintVersion();
    return 0;
    }

  if( help )
    {
    PrintHelp();
    return 0;
    }
  
  const char *file1extension = file1.GetExtension();
  //const char *file2extension = file2.GetExtension();

  if(gdcm::System::StrCaseCmp(file1extension,".xml") != 0)// by default we assume it is a DICOM file-- as no extension is required for it
    {
    gdcm::Reader reader;
    reader.SetFileName( file1.GetFileName() );
    bool success = reader.Read();
    if( !success )//!ignoreerrors )
      {
      std::cerr << "Failed to read: " << file1 << std::endl;
      return 1;
      }

    //SimpleFileXMLPrinter printer;
    XMLPrinter printer;
    printer.SetFile ( reader.GetFile() );
    printer.SetStyle ( (XMLPrinter::PrintStyles)loadBulkData );

    if( file2.IsEmpty() )
      {
      printer.Print( std::cout );
      }
    else
      {
      std::ofstream outfile;
      outfile.open (file2.GetFileName());
      printer.Print( outfile );
      outfile.close();
      }
    return 0;
    }
  else
    {
#ifdef GDCM_USE_SYSTEM_LIBXML2
    /*
     * this initialize the library and check potential ABI mismatches
     * between the version it was compiled for and the actual shared
     * library used.
     */
    LIBXML_TEST_VERSION

    XMLtoDICOM(file1,file2);

    /*
     * Cleanup function for the XML library.
     */
    xmlCleanupParser();
    /*
     * this is to debug memory for regression tests
     */
    xmlMemoryDump();
#else
		printf("\nPlease configure Cmake options with GDCM_USE_SYSTEM_LIBXML2 as ON and compile!\n");    
#endif
    }
}
