//-----------------------------------------------------------------------------
// Created on: 15 April 2017
// Created by: Quaoar
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/, http://quaoar.su/blog
//-----------------------------------------------------------------------------

#ifndef asiVisu_PartPipelineBase_h
#define asiVisu_PartPipelineBase_h

// A-Situs includes
#include <asiVisu_DisplayModeFilter.h>
#include <asiVisu_Pipeline.h>
#include <asiVisu_Selection.h>
#include <asiVisu_ShapeRobustSource.h>

// Active Data includes
#include <ActAPI_IPlotter.h>
#include <ActAPI_IProgressNotifier.h>

// VTK includes
#include <vtkPolyDataNormals.h>

//-----------------------------------------------------------------------------

DEFINE_STANDARD_HANDLE(asiVisu_PartPipelineBase, asiVisu_Pipeline)

//! Base class for all pipelines associated with Part Node.
class asiVisu_PartPipelineBase : public asiVisu_Pipeline
{
public:

  // OCCT RTTI
  DEFINE_STANDARD_RTTI_INLINE(asiVisu_PartPipelineBase, asiVisu_Pipeline)

public:

  asiVisu_EXPORT
    asiVisu_PartPipelineBase(const vtkSmartPointer<asiVisu_ShapeRobustSource>& source);

public:

  asiVisu_EXPORT void
    SetDiagnosticTools(ActAPI_ProgressEntry progress,
                       ActAPI_PlotterEntry  plotter);

  asiVisu_EXPORT void
    SetPickedElements(const TColStd_PackedMapOfInteger& elementIds,
                      const asiVisu_SelectionNature     selNature);

  asiVisu_EXPORT void
    ResetPickedElements(const asiVisu_SelectionNature selNature);

public:

  //! \return data source.
  const vtkSmartPointer<asiVisu_ShapeRobustSource>& GetSource() const
  {
    return m_source;
  }

  //! \return normals calculation filter.
  const vtkSmartPointer<vtkPolyDataNormals>& GetNormalsFilter() const
  {
    return m_normalsFilter;
  }

  //! \return display mode filter.
  const vtkSmartPointer<asiVisu_DisplayModeFilter>& GetDisplayModeFilter() const
  {
    return m_dmFilter;
  }

private:

  virtual void callback_add_to_renderer      (vtkRenderer* renderer);
  virtual void callback_remove_from_renderer (vtkRenderer* renderer);
  virtual void callback_update               ();

private:

  //! Copying prohibited.
  asiVisu_PartPipelineBase(const asiVisu_PartPipelineBase&);

  //! Assignment prohibited.
  asiVisu_PartPipelineBase& operator=(const asiVisu_PartPipelineBase&);

protected:

  //! Clears internal cache data maps.
  void clearCache()
  {
    m_detectedCells.Clear();
    m_selectedCells.Clear();
  }

protected:

  //! Technical flag indicating whether a GL-mapper is initialized with the
  //! custom color scheme or not.
  bool m_bMapperColorsSet;

  //! Data source.
  vtkSmartPointer<asiVisu_ShapeRobustSource> m_source;

  //! Filter for normals.
  vtkSmartPointer<vtkPolyDataNormals> m_normalsFilter;

  //! Display mode filter.
  vtkSmartPointer<asiVisu_DisplayModeFilter> m_dmFilter;

  //! Progress notifier.
  ActAPI_ProgressEntry m_progress;

  //! Imperative plotter.
  ActAPI_PlotterEntry m_plotter;

  //! Data map of currently DETECTED cell IDs (for fast access). The value
  //! is the original scalar.
  NCollection_DataMap<vtkIdType, int> m_detectedCells;

  //! Data map of currently SELECTED cell IDs (for fast access). The value
  //! is the original scalar.
  NCollection_DataMap<vtkIdType, int> m_selectedCells;

};

#endif