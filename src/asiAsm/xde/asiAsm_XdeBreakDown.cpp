//-----------------------------------------------------------------------------
// Created on: 07 March 2024
//-----------------------------------------------------------------------------
// Copyright (c) 2024-present, Julia Slyadneva
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
//    * Redistributions of source code must retain the above copyright
//      notice, this list of conditions and the following disclaimer.
//    * Redistributions in binary form must reproduce the above copyright
//      notice, this list of conditions and the following disclaimer in the
//      documentation and/or other materials provided with the distribution.
//    * Neither the name of the copyright holder(s) nor the
//      names of all contributors may be used to endorse or promote products
//      derived from this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
// ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
// WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
// DISCLAIMED. IN NO EVENT SHALL THE AUTHORS OR CONTRIBUTORS BE LIABLE FOR ANY
// DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
// (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
// LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
// ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
// SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//-----------------------------------------------------------------------------

// Own include
#include <asiAsm_XdeBreakDown.h>

// asiAlgo includes
#include <asiAlgo_Utils.h>
#include <asiAlgo_STEP.h>

// OCCT includes
#include <OSD_File.hxx>

using namespace asiAsm::xde;
using namespace asiAlgo;

//-----------------------------------------------------------------------------

BreakDown::BreakDown(ActAPI_ProgressEntry progress,
                     ActAPI_PlotterEntry  plotter)
//
: ActAPI_IAlgorithm  (progress, plotter),
  m_bExpandCompounds (false)
{
}

//-----------------------------------------------------------------------------

bool BreakDown::Perform(const Handle(Doc)&   xdeDoc,
                        const t_asciiString& outputDir)
{
  // BOM filename.
  std::string bomFilename = asiAlgo_Utils::Str::Slashed( outputDir.ToCString() );
  //
  if ( !m_prefix.IsEmpty() )
  {
    bomFilename += m_prefix.ToCString();
    bomFilename += "_";
  }
  //
  bomFilename += "bom.csv";

  // Filenames used for unique parts.
  NCollection_DataMap<PartId, std::string, PartId::Hasher> partFilenames;

  // Expand compound parts.
  if ( m_bExpandCompounds )
  {
    AssemblyItemIds leaves;
    xdeDoc->GetLeafAssemblyItems(leaves);
    //
    xdeDoc->ExpandCompounds(leaves);
  }

  // Get unique parts.
  PartIds pids;
  xdeDoc->GetParts(pids);

  // Count parts.
  NCollection_DataMap<PartId, int, PartId::Hasher> partQuantities;
  xdeDoc->CountParts(partQuantities);

  // Create file for BOM output.
  std::ofstream bomFile;
  bomFile.open(bomFilename, std::ios::out | std::ios::trunc);

  // Add header to BOM.
  bomFile << "id"       << ","
          << "filename" << ","
          << "quantity" << "\n";

  m_progress.Reset();
  m_progress.Init(pids.Size());

  // Iterate over the unique parts.
  for ( PartIds::Iterator pit(pids); pit.More(); pit.Next() )
  {
    int nextUniqueId = 1;

    // Next part.
    const PartId& pid       = pit.Value();
    TopoDS_Shape  partShape = xdeDoc->GetShape(pid);
    t_extString   partName  = xdeDoc->GetPartName(pid);

    // Remove unacceptable characters.
    partName.RemoveAll( '<' );
    partName.RemoveAll( '>' );
    partName.RemoveAll( '/' );
    partName.RemoveAll( '\\' );

    // Prepare a filename.
    std::string filename = asiAlgo_Utils::Str::Slashed( outputDir.ToCString() );
    //
    if ( !m_prefix.IsEmpty() )
    {
      filename += m_prefix.ToCString();
      filename += "_";
    }
    filename += TCollection_AsciiString(partName).ToCString();
    filename += ".stp";

    // Make sure that such a file does not exist yet.
    while ( OSD_File( OSD_Path( filename.c_str() ) ).Exists() )
    {
      m_progress.SendLogMessage(LogWarn(Normal) << "Filename '%1' has already been used."
                                                << filename);

      // Recompose the filename with unique index.
      filename = asiAlgo_Utils::Str::Slashed( outputDir.ToCString() );
      //
      if ( !m_prefix.IsEmpty() )
      {
        filename += m_prefix.ToCString();
        filename += "_";
      }
      //
      filename += TCollection_AsciiString(partName).ToCString();
      filename += asiAlgo_Utils::Str::ToString(nextUniqueId++);
      filename += ".stp";
    }

    // Write STEP file.
    asiAlgo_STEP stepWriter( m_progress );
    //
    if ( !stepWriter.Write( partShape, filename.c_str() ) )
    {
      m_progress.SendLogMessage(LogErr(Normal) << "Cannot export part %1 to '%2'."
                                               << pid << filename);
      continue;
    }

    // Keep track of filenames.
    partFilenames.Bind( pid, asiAlgo_Utils::Str::BaseFilename(filename, true) );

    // Add to the BOM file.
    bomFile << pid.ToString()      << ","
            << partFilenames(pid)  << ","
            << partQuantities(pid) << "\n";

    m_progress.StepProgress(1);
  }

  bomFile.close();
  m_progress.SetProgressStatus(ActAPI_ProgressStatus::Progress_Succeeded);

  return true;
}
