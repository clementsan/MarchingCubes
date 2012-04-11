#include <iostream>

#include <itkImage.h>
#include <itkImageFileReader.h>
#include "itkImageToVTKImageFilter.h"

#include <vtkImageMarchingCubes.h>
#include <vtkPolyDataWriter.h>

typedef float PixelType;
typedef itk::Image< PixelType, 3 > ImageType;
typedef itk::ImageFileReader< ImageType  >  ReaderType;
typedef itk::ImageToVTKImageFilter<ImageType> Itk2VtkType;
      
using namespace std;

void DisplayHelp(char *ProgName);

int main (int argc, char *argv[])
{
  if (argc != 3 || ((argc==1) && (!strcmp(argv[1],"-h"))))
    {
      DisplayHelp(argv[0]);
      return EXIT_FAILURE;
    }  
  else
    {
      ImageType::Pointer image ;
      ReaderType::Pointer imageReader = ReaderType::New() ;
      try
	{
	  imageReader->SetFileName(argv[1]) ;
	  imageReader->Update() ;
	  image = imageReader->GetOutput() ;
	}
      catch( itk::ExceptionObject exp )
	{
	  cerr<<"Exception caught !"<<endl;
	  cerr<<exp<<endl;
	}
      
      // go to vtk 
      Itk2VtkType::Pointer  m_Itk2Vtk = Itk2VtkType::New();      
      m_Itk2Vtk->SetInput(image);  
      m_Itk2Vtk->Update();
      
      // create mesh using marching cubes
      vtkImageMarchingCubes *marcher = vtkImageMarchingCubes::New();
      marcher->SetInput(m_Itk2Vtk->GetOutput());
      marcher->SetValue(0, 0.5);
      marcher->SetNumberOfContours(1);
      marcher->ComputeNormalsOff();
      marcher->ComputeScalarsOff();
      marcher->ComputeGradientsOff();
      marcher->Update() ;  
      vtkPolyData *pipePolyTail = marcher->GetOutput();
      
      // write out the vtk mesh
      vtkPolyDataWriter *writer = vtkPolyDataWriter::New();
      writer->SetInput(pipePolyTail);
      writer->SetFileName(argv[2]);
      writer->Update();
    }

  return EXIT_SUCCESS;
}

void DisplayHelp(char *ProgName)
{
  cerr<<ProgName<<endl;
  cerr<<"usage: "<<ProgName<<" <InputImage> <OutputSurface>"<<endl;
  cerr<<"  OutputSurface: vtk file"<<endl;  
}
