/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $HeadURL$
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#include <itkPluginUtilities.h>
#include "LabelStatisticsExtractionCLP.h"

#include <itkImageFileReader.h>

#include <itkLabelStatisticsImageFilter.h>
#include <itkImageRegionIterator.h>

// Use an anonymous namespace to keep class types and function names
// from colliding when module is used as shared object module.  Every
// thing should be in an anonymous namespace except for the module
// entry point, e.g. main()
//
namespace
{

template <class T>
int DoIt( int argc, char * argv[], T )
{
    PARSE_ARGS;

    typedef    T InputPixelType;

    typedef itk::Image<InputPixelType,  3> ImageType;
    typedef itk::Image<unsigned char,  3> LabelType;

    typedef itk::ImageFileReader<ImageType>  ReaderType;
    typedef itk::ImageFileReader<LabelType>  l_ReaderType;

    typename ReaderType::Pointer readerImage = ReaderType::New();
    typename l_ReaderType::Pointer readerLabel = l_ReaderType::New();

    readerImage->SetFileName( inputVolume.c_str() );
    readerImage->ReleaseDataFlagOn();

    readerImage->Update();

    readerLabel->SetFileName( inputLabel.c_str() );
    readerLabel->ReleaseDataFlagOn();

    readerLabel->Update();

    // Filters label for image values of 0
    typedef itk::ImageRegionIterator<ImageType> ImageIteratorType;
    typedef itk::ImageRegionIterator<LabelType> LabelIteratorType;

    ImageIteratorType imgIt(readerImage->GetOutput(), readerImage->GetOutput()->GetRequestedRegion());
    LabelIteratorType lblIt(readerLabel->GetOutput(), readerLabel->GetOutput()->GetRequestedRegion());

    lblIt.GoToBegin();
    while(!lblIt.IsAtEnd()){
        imgIt.SetIndex(lblIt.GetIndex());
        if(lblIt.Get()>0 && imgIt.Get()==0)
            lblIt.Set(0);
        ++lblIt;
    }

    // Computes the mean and variance for labels
    typedef itk::LabelStatisticsImageFilter<ImageType, LabelType> StatisticsFilterType;
    typename StatisticsFilterType::Pointer statistics = StatisticsFilterType::New();

    statistics->SetInput(readerImage->GetOutput());
    statistics->SetLabelInput(readerLabel->GetOutput());
    statistics->Update();

    typedef typename StatisticsFilterType::ValidLabelValuesContainerType ValidLabelValuesType;
    typedef typename StatisticsFilterType::LabelPixelType                LabelPixelType;

    for(typename ValidLabelValuesType::const_iterator vIt = statistics->GetValidLabelValues().begin();
        vIt != statistics->GetValidLabelValues().end();
        ++vIt){
        LabelPixelType labelValue = *vIt;
        int i = (int) labelValue;
        double mean = statistics->GetMean(i);
        double var = statistics->GetVariance(i);
        double std = statistics->GetSigma(i);
        double max = statistics->GetMaximum(i);
        double min = statistics->GetMinimum(i);
        std::cout<<i<<" "<<mean<<" "<<var<<" "<<std<<" "<<max<<" "<<min<<std::endl;
    }

    return EXIT_SUCCESS;
}

} // end of anonymous namespace

int main( int argc, char * argv[] )
{

    PARSE_ARGS;

    itk::ImageIOBase::IOPixelType     pixelType;
    itk::ImageIOBase::IOComponentType componentType;

    try
    {
        itk::GetImageType(inputVolume, pixelType, componentType);

        switch( componentType )
        {
        case itk::ImageIOBase::UCHAR:
            return DoIt( argc, argv, static_cast<unsigned char>(0) );
            break;
        case itk::ImageIOBase::CHAR:
            return DoIt( argc, argv, static_cast<char>(0) );
            break;
        case itk::ImageIOBase::USHORT:
            return DoIt( argc, argv, static_cast<unsigned short>(0) );
            break;
        case itk::ImageIOBase::SHORT:
            return DoIt( argc, argv, static_cast<short>(0) );
            break;
        case itk::ImageIOBase::UINT:
            return DoIt( argc, argv, static_cast<unsigned int>(0) );
            break;
        case itk::ImageIOBase::INT:
            return DoIt( argc, argv, static_cast<int>(0) );
            break;
        case itk::ImageIOBase::ULONG:
            return DoIt( argc, argv, static_cast<unsigned long>(0) );
            break;
        case itk::ImageIOBase::LONG:
            return DoIt( argc, argv, static_cast<long>(0) );
            break;
        case itk::ImageIOBase::FLOAT:
            return DoIt( argc, argv, static_cast<float>(0) );
            break;
        case itk::ImageIOBase::DOUBLE:
            return DoIt( argc, argv, static_cast<double>(0) );
            break;
        case itk::ImageIOBase::UNKNOWNCOMPONENTTYPE:
        default:
            std::cout << "unknown component type" << std::endl;
            break;
        }

        // This filter handles all types on input, but only produces
        // signed types

    }
    catch( itk::ExceptionObject & excep )
    {
        std::cerr << argv[0] << ": exception caught !" << std::endl;
        std::cerr << excep << std::endl;
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}
