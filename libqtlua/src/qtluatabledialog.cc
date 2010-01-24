/*
    This file is part of LibQtLua.

    LibQtLua is free software: you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    LibQtLua is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU Lesser General Public License
    along with LibQtLua.  If not, see <http://www.gnu.org/licenses/>.

    Copyright (C) 2010, Alexandre Becoulet <alexandre.becoulet@free.fr>

*/

#include <QDialogButtonBox>
#include <QVBoxLayout>
#include <QTreeView>
#include <QTableView>
#include <QHeaderView>
#include <QPushButton>
#include <QResizeEvent>

#include <QtLua/TableDialog>

namespace QtLua {

  TableDialog::TableDialog(const Value &root, enum ViewType type,
			   QAbstractItemModel *model,
			   int attr, QWidget *parent)
    : QDialog(parent),
      _model(model),
      _eb(0), _rb(0), _ib(0) //, _rc(0), _ic(0)
  {
    QDialogButtonBox *buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok);
    connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));

    switch (type)
      {
      case TreeTreeView: {
	if (!_model)
	  _model = new TableTreeModel(root, TableTreeModel::Attributes(attr), this);
	QTreeView *view = new QTreeView();
	view->setRootIsDecorated(attr & TableTreeModel::Recursive);
	_view = view;
	break;
      }

      case TreeTableView: {
	if (!_model)
	  _model = new TableTreeModel(root, TableTreeModel::Attributes(attr), this);
	QTableView *view = new QTableView();
	view->verticalHeader()->hide();
	_view = view;
	break;
      }

      case GridTableView: {
	if (!_model)
	  _model = new TableGridModel(root, TableGridModel::Attributes(attr), true, this);
	QTableView *view = new QTableView();
	_view = view;
	break;
      }

      }

    _view->setModel(_model);

    switch (type)
      {
      case TreeTreeView:
      case TreeTableView: {

	if (attr & TableTreeModel::Editable)
	  {
	    _eb = buttonBox->addButton(tr("Edit"), QDialogButtonBox::ActionRole);
	    _eb->setEnabled(false);
	    connect(_eb, SIGNAL(clicked()), this, SLOT(edit()));
	  }

	if (attr & TableTreeModel::EditRemove)
	  {
	    _rb = buttonBox->addButton(tr("Remove"), QDialogButtonBox::ActionRole);
	    _rb->setEnabled(false);
	    connect(_rb, SIGNAL(clicked()), this, SLOT(tree_remove()));
	  }

	if (attr & TableTreeModel::EditInsert)
	  {
	    _ib = buttonBox->addButton(tr("Insert"), QDialogButtonBox::ActionRole);
	    connect(_ib, SIGNAL(clicked()), this, SLOT(tree_insert()));
	  }

	connect(_view->selectionModel(), SIGNAL(currentChanged(const QModelIndex&, const QModelIndex&)),
		this, SLOT(tree_current_changed(const QModelIndex&)));

	break;
      }

      case GridTableView: {

	if (attr & TableGridModel::Editable)
	  {
	    _eb = buttonBox->addButton(tr("Edit"), QDialogButtonBox::ActionRole);
	    _eb->setEnabled(false);
	    connect(_eb, SIGNAL(clicked()), this, SLOT(edit()));
	  }

	if (attr & TableGridModel::EditRemoveRow)
	  {
	    _rb = buttonBox->addButton(tr("Remove row"), QDialogButtonBox::ActionRole);
	    _rb->setEnabled(false);
	    connect(_rb, SIGNAL(clicked()), this, SLOT(grid_remove_row()));
	  }

	if (attr & TableGridModel::EditInsertRow)
	  {
	    _ib = buttonBox->addButton(tr("Insert row"), QDialogButtonBox::ActionRole);
	    connect(_ib, SIGNAL(clicked()), this, SLOT(grid_insert_row()));
	  }

#if 0
	if (attr & TableGridModel::EditRemoveCol)
	  {
	    _rc = buttonBox->addButton(tr("Remove column"), QDialogButtonBox::ActionRole);
	    _rc->setEnabled(false);
	    connect(_rc, SIGNAL(clicked()), this, SLOT(grid_remove_col()));
	  }

	if (attr & TableGridModel::EditInsertCol)
	  {
	    _ic = buttonBox->addButton(tr("Insert column"), QDialogButtonBox::ActionRole);
	    _ic->setEnabled(false);
	    connect(_ic, SIGNAL(clicked()), this, SLOT(grid_insert_col()));
	  }
#endif

	connect(_view->selectionModel(), SIGNAL(currentChanged(const QModelIndex&, const QModelIndex&)),
		this, SLOT(grid_current_changed(const QModelIndex&)));

	break;
      }

      }

    QVBoxLayout *layout = new QVBoxLayout;
    layout->addWidget(_view);
    layout->addWidget(buttonBox);
    setLayout(layout);
  }

  void TableDialog::edit() const
  {
    QModelIndex index = _view->currentIndex();

    if (index.isValid())
      {
	_view->scrollTo(index);
	_view->edit(index);
      }
  }

  void TableDialog::tree_insert() const
  {
    QModelIndex index = _view->currentIndex();
    QModelIndex parent;
    int row = 0;

    if (index.isValid())
      {
	parent = _model->parent(index);
	row = index.row();
      }

    _model->insertRow(row, parent);
    index = _model->index(row, TableTreeModel::ColKey, parent);

    if (index.isValid())
      {
	_view->scrollTo(index);
	_view->edit(index);
      }
  }

  void TableDialog::tree_remove() const
  {
    QModelIndex index = _view->currentIndex();

    if (index.isValid())
      _model->removeRow(index.row(), _model->parent(index));
  }


  void TableDialog::tree_current_changed(const QModelIndex &index) const
  {
    TableTreeModel::Attributes attr = static_cast<TableTreeModel*>(_model)->get_attr(index);

    if (_rb)
      _rb->setEnabled(index.isValid() && (attr & TableTreeModel::EditRemove));

    if (_ib)
      _ib->setEnabled(attr & TableTreeModel::EditInsert);

    if (_eb)
      _eb->setEnabled(_model->flags(index) & Qt::ItemIsEditable);
  }


  void TableDialog::grid_current_changed(const QModelIndex &index) const
  {
    if (_rb)
      _rb->setEnabled(index.isValid());

#if 0
    if (_rc)
      _rc->setEnabled(index.isValid());
    if (_ic)
      _ic->setEnabled(index.isValid());
#endif

    if (_eb)
      _eb->setEnabled(index.isValid());
  }

  void TableDialog::grid_insert_row() const
  {
    QModelIndex index = _view->currentIndex();
    int row = _model->columnCount(QModelIndex());

    // FIXME should be able to insert as last row
    if (index.isValid())
      row = index.row();

    _model->insertRow(row, QModelIndex());

    index = _model->index(row, 0, QModelIndex());
    if (index.isValid())
      _view->scrollTo(index);
  }

  void TableDialog::grid_remove_row() const
  {
    QModelIndex index = _view->currentIndex();

    if (index.isValid())
      _model->removeRow(index.row(), QModelIndex());
  }

#if 0
  void TableDialog::grid_insert_col() const
  {
    QModelIndex index = _view->currentIndex();

    _model->insertColumn(index.column(), QModelIndex());
    _view->scrollTo(index);
  }

  void TableDialog::grid_remove_col() const
  {
    QModelIndex index = _view->currentIndex();

    if (index.isValid())
      _model->removeColumn(index.column(), QModelIndex());
  }
#endif

  void TableDialog::tree_tree_dialog(QWidget *parent, const QString &title, const Value &table, 
				     TableTreeModel::Attributes attr)
  {
    TableDialog d(table, TreeTreeView, 0, attr, parent);
    d.setWindowTitle(title);
    d.exec();
  }

  void TableDialog::tree_table_dialog(QWidget *parent, const QString &title, const Value &table, 
				      TableTreeModel::Attributes attr)
  {
    TableDialog d(table, TreeTableView, 0, attr, parent);
    d.setWindowTitle(title);
    d.exec();
  }

  void TableDialog::grid_table_dialog(QWidget *parent, const QString &title, const Value &table, 
				      TableGridModel::Attributes attr,
				      const Value::List *colkeys, const Value::List *rowkeys)
  {
    TableGridModel model(table, attr, false, 0);

    if (rowkeys)
      foreach(const Value &k, *rowkeys)
	model.add_row_key(k);
    else
      model.fetch_all_row_keys();

    if (colkeys)
      foreach(const Value &k, *colkeys)
	model.add_column_key(k);
    else
      model.fetch_all_column_keys();

    TableDialog d(table, GridTableView, &model, attr, parent);
    d.setWindowTitle(title);
    d.exec();
  }

  QSize TableDialog::sizeHint() const
  {
    int colcount = _model->columnCount(QModelIndex());
    QSize hint(0, 640);

    if (QTreeView *tv = dynamic_cast<QTreeView*>(_view))
      {
	for (int i = 0; i < colcount; i++)
	  {
	    tv->resizeColumnToContents(i);
	    // Leave room for expand
	    if (i == TableTreeModel::ColKey)
	      tv->setColumnWidth(i, tv->columnWidth(i) * 1.5);

	    hint.rwidth() += tv->columnWidth(i);
	  }
      }
    else if (QTableView *tv = dynamic_cast<QTableView*>(_view))
      {
	for (int i = 0; i < colcount; i++)
	  {
	    tv->resizeColumnToContents(i);
	    hint.rwidth() += tv->columnWidth(i);
	  }
      }

    return hint;
  }

}

