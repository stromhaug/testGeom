//-----------------------------------------------------------------------------
// Created on: 26 February 2023
//-----------------------------------------------------------------------------
// Copyright (c) 2023-present, Anton Poletaev, Sergey Slyadnev
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

#ifndef asiUI_DatumTable_HeaderFile
#define asiUI_DatumTable_HeaderFile

// asiUI includes
#include <asiUI_DatumViewApi.h>
#include <asiUI_DatumViewBase.h>

// Qt includes
#pragma warning(push, 0)
#include <QTableView>
#pragma warning(pop)

#pragma warning(disable: 4251) // "Class XXX needs to have dll-interface..."

// Forward declaration
class QHeaderView;
class asiUI_DatumViewItem;

//-----------------------------------------------------------------------------

//! Table uses datum editors for editing its cell items.
//!
//! The table allows setting datum editors for the already existing
//! columns and rows.
//! \sa SetTableEditor, SetRowEditor, SetColumnEditor, SetCellEditor.
//!
//! The column (row) header displays dictionary label and unit string of the
//! corresponding column(row) datum editor. There is an optional flag
//! that allows turning off/on unit strings in the headers.
//! \sa IsShowUnitsInLabel().
//!
//! It is also possible to set user-defined label for each column or row instead
//! of the one generated by datum editors.
//! \sa SetColumnTitles(...), SetRowTitles(...)
//!
//! Single-click editing is enabled by default, this feature can be turned off
//! by setting a Boolean flag with SetEditByClick(...) method.
//! \sa SetEditByClick(...).
class asiUI_EXPORT asiUI_DatumTable : public asiUI_DatumViewBase<QTableView>,
                                      public asiUI_DatumViewApi
{
  Q_OBJECT

// internal types
private:

  typedef QVector<QString> VectorOfStrings;
  typedef QVector<QString> VectorOfDatums;
  struct ArrayOfCellDatums
  {
  public:
    ArrayOfCellDatums();
    void Resize(const int theRows, const int theCols);
    void InsertRows(const int theAt, const int theRows);
    void InsertCols(const int theAt, const int theCols);
    void RemoveRows(const int theAt, const int theRows);
    void RemoveCols(const int theAt, const int theRows);
    void Set(const int theRow, const int theCol, const QString& theDatum);
    QString Get(const int theRow, const int theCol) const;
  private:
    int m_rows;
    int m_cols;
    QVector<VectorOfDatums> m_datums;
  };

  struct EditorSet
  {
  public:
    int  Rows() const;
    int  Cols() const;
    void Resize(const int theRows, const int theCols);
    void InsertRows(const int theAt, const int theRows);
    void InsertCols(const int theAt, const int theCols);
    void RemoveRows(const int theAt, const int theRows);
    void RemoveCols(const int theAt, const int theCols);
    void SetDefaultEditor(const QString& theDatum);
    void SetRowEditor(const int theRow, const QString& theDatum);
    void SetColEditor(const int theCol, const QString& theDatum);
    void SetItemEditor(const int theRow, const int theCol, const QString& theDatum);
    const QString& GetDefaultEditor() const;
    QString GetRowEditor(const int theRow) const;
    QString GetColEditor(const int theCol) const;
    QString GetEditor(const int theRow, const int theCol) const;
  private:
    QString           m_defaultDatum;
    VectorOfDatums    m_rowDatums;
    VectorOfDatums    m_colDatums;
    ArrayOfCellDatums m_itemDatums;
  };

public:

  //! Kind of edit cursor propagation
  //! between cells when return key
  //! pressed or editing of item is
  //! finished
  enum CursorPropagation
  {
    Hold,
    LeftToRight,
    UpToDown
  };

//! Construction
public:

  asiUI_DatumTable(const Handle(asiUI_WidgetFactory)& theFactory,
                   QWidget* theParent = 0);

  ~asiUI_DatumTable();

// public API implementation
public:

  virtual QVariant
    GetValue(const int theRow,
             const int theColumn) const;

  virtual void
    SetValue(const int theRow,
             const int theColumn,
             const QVariant& theValue);

  virtual QString
    GetString(const int theRow,
              const int theColumn) const;

  virtual void
    SetString(const int theRow,
              const int theColumn,
              const QString& theString);

  virtual int
    GetRowCount() const;

  virtual int
    GetColumnCount() const;

  virtual void
    SetRowCount(const int theRows);

  virtual void
    SetColumnCount(const int theCols);

  virtual asiUI_DatumViewItem*
    AccessItem(const int theRow,
               const int theCol) const;

  virtual void
    SetItemPrototype(const asiUI_DatumViewItem* thePrototype);

  virtual asiUI_DatumViewApi::IndexList
    GetSelectedIndexes() const;

  virtual bool
    SelectIndexes(const asiUI_DatumViewApi::IndexList &theList);

  virtual ItemIndex
    MapViewToItem(const QModelIndex& theIndex) const;

  virtual QModelIndex
    MapItemToView(const int theRow, const int theCol) const;

public slots:

  virtual void
    InsertRows(const int theAt,
               const int theRows = 1);

  virtual void
    InsertColumns(const int theAt,
                  const int theCols = 1);

  virtual void
    RemoveRows(const int theAt,
               const int theRows = 1);

  virtual void
    RemoveColumns(const int theAt,
                  const int theCols = 1);

public:

  virtual QAbstractItemView*
    Widget();

// Edit cursor propagation
public:

  void SetCursorPropagation(const CursorPropagation theDir);

  CursorPropagation GetCursorPropagation() const;

// label and headers manipulation
public:

  void SetColumnTitles(const QStringList& theTitle);

  void SetRowTitles(const QStringList& theTitle);

  QString GetColumnTitle(const int theColumn) const;

  void SetColumnTitle(const int theColumn,  const QString& theTitle);

  QString GetRowTitle(const int theRow) const;

  void SetRowTitle(const int theRow, const QString& theTitle);

// header extensions
public:

  void AddHeaderGroupSection(const Qt::Orientation theHeader,
                             const asiUI_HeaderIndex& theGroup);

  void RemoveHeaderGroupSection(const Qt::Orientation theHeader,
                                const asiUI_HeaderIndex& theGroup);

  void SetHeaderItemData(const Qt::Orientation theHeader,
                         const asiUI_HeaderIndex& theItemIndex,
                         const QVariant& theData,
                         const int theRole = DatumViewRole_LocalValue);

  QVariant HeaderItemData(const Qt::Orientation theHeader,
                          const asiUI_HeaderIndex& theIndex,
                          const int theRole = DatumViewRole_FormattedValue) const;

  void SetHeaderItem(const Qt::Orientation theHeader,
                     const asiUI_HeaderIndex& theIndex,
                     QStandardItem* theItem);

  QStandardItem* HeaderItem(const Qt::Orientation theHeader,
                            const asiUI_HeaderIndex& theIndex) const;

  void SetHeaderBandsCount(const Qt::Orientation theHeader,
                           const int theRows);

  Standard_Integer GetHeaderBandsCount(const Qt::Orientation theHeader) const;

  void SetHeaderBandDatum(const Qt::Orientation theHeader,
                          const QString& theDatum);

  void SetHeaderBandSectDatum(const Qt::Orientation theHeader,
                              const QString& theDatum,
                              const int theHeaderSectIdx);

  void SetHeaderBandLineDatum(const Qt::Orientation theHeader,
                              const QString& theDatum,
                              const int theBandLineIndex);

  void SetHeaderBandDatum(const Qt::Orientation theHeader,
                          const QString& theDatum,
                          const asiUI_HeaderIndex& theIndex);

  void ResetHierarchyHeader(const Qt::Orientation theHeader = Qt::Horizontal);

  void ResetBandRowHeader(const Qt::Orientation theHeader = Qt::Horizontal);

// datum editor id setters/getters
public:

  QString GetTableEditor() const;

  void SetTableEditor(const QString& theDatum);

  QString GetRowEditor(const int theRow) const;

  void SetRowEditor(const int theRow, const QString& theDatum);

  QString GetColumnEditor(const int theCol) const;

  void SetColumnEditor(const int theCol, const QString& theDatum);

  QString GetCellEditor(const int theRow, const int theCol) const;

  void SetCellEditor(const int theRow, const int theCol, const QString& theDatum);

// Corner widget
public:

  void SetHeaderCornerWidget(QWidget* theWidget);

  QWidget* GetHeaderCornerWidget() const;

// layout control methods
public:

  void resetHeaders(const Qt::Orientation& theOrientation,
                    const int theStartIdx,
                    const int theEndIdx);

  void updateHeaders(const Qt::Orientation& theOrientation,
                     const int theStartIdx,
                     const int theEndIdx);

  virtual void
    updateGeometries();

  virtual void
    doItemsLayout();

signals:

  //! The signal is emitted when value is changed
  //! either automatically or by the user input.
  //! \param theRow [in] the row index of modified item.
  //! \param theColumn [in] the column index of modified item.
  //! \param theValue [in] the new value.
  void ValueChanged(const int theRow,
                    const int theColumn,
                    const QVariant& theValue);

  //! Emitted when modification is started on cell's value.
  //! \param theRow [in] the row index of the item being modified.
  //! \param theColumn [in] the column index of the item being modified.
  //! \param theWidget [in] created widget.
  void ValueChanging(const int theRow,
                     const int theColumn,
                     QWidget* theWidget);

  //! Emitted when Datum used for cell modification is closed.
  //! \param theRow [in] the row index of the item being modified.
  //! \param theColumn [in] the column index of the item being modified.
  //! \param theWidget [in] closed widget.
  void DatumClosed(const int theRow,
                   const int theColumn,
                   QWidget* theWidget);

  //! Emitted when selection on Datum Table is changed.
  //! \param theSelected [in] new selection range.
  //! \param theDeselected [in] former selection range.
  void SelectionChanged(const QItemSelection& theSelected,
                        const QItemSelection& theDeselected);

  //! Emitted when custom selector is triggered for the item.
  //! \param theRow [in] the row index of the triggered item.
  //! \param theColumn [in] the column index of the triggered item.
  void CustomSelectorTriggered(const int theRow,
                               const int theColumn);

protected slots:

  void onHeaderDataChanged(Qt::Orientation theOrientation,
                           int theFirst,
                           int theLast);

  void onColumnsInserted(QModelIndex theParent, int theStart, int theEnd);

  void onColumnsRemoved(QModelIndex theParent, int theStart, int theEnd);

  void onRowsInserted(QModelIndex theParent, int theStart, int theEnd);

  void onRowsRemoved(QModelIndex theParent, int theStart, int theEnd);

  void ContextMenu(QPoint pos);

  void CopyClipboardData();

  void PasteClipboardData();

protected:

  virtual QString 
    getItemDatum(const QModelIndex& theIndex) const;

  virtual QString
    getHeaderDatum(const Qt::Orientation theHeader,
                   const asiUI_HeaderIndex& theIndex) const;

  virtual void
    refreshUnitsInLabels();

  virtual void
    keyPressEvent(QKeyEvent *theEvent);

  virtual void
    emitValueChanged(const QModelIndex& theIndex,
                     const QVariant& theValue);

  virtual void
    emitValueChanging(const QModelIndex& theIndex,
                      QWidget* theWidget);

  virtual void
    emitDatumClosed(const QModelIndex& theIndex,
                    QWidget* theWidget);

  virtual void
    emitSelectionChanged(const QItemSelection& theSelected,
                         const QItemSelection& theDeselected);

  virtual void
    emitCustomSelectorTriggered(const QModelIndex& theIndex);

protected:

  virtual void
    setModel(QAbstractItemModel* theModel);

  virtual QModelIndex
    moveCursor(CursorAction theAction,
               Qt::KeyboardModifiers theModifiers);

  void updateItems(const int  theStartRow,
                   const int  theEndRow,
                   const int  theStartColumn,
                   const int  theEndColumn,
                   const bool theSkipEdited = false);

  QHeaderView* header(const Qt::Orientation theOrientation) const;

protected slots:

  virtual void selectAll();

// internal data types
private:

  typedef QVector<QString> StringVector;
  typedef QVector<QVector<QString>> DoubleStringVector;

private:

  EditorSet                              m_ViewEditors;
  CursorPropagation                      m_CursorProp;
  QMap<Qt::Orientation, EditorSet>       m_BandEditors;
  QMap<Qt::Orientation, VectorOfStrings> m_Titles;
  QWidget*                               m_CornerWidget;
  bool                                   m_bGeomUpdateBlock;
};

#pragma warning(default: 4251) // "Class XXX needs to have dll-interface..."

#endif
