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

#include <QtLua/TableDialog>

namespace QtLua {

  TableDialog::TableDialog(QWidget *parent, const Value &root,
			   TableModel::Attributes attr, bool tableview)
    : QDialog(parent),
      _model(new TableModel(root, 0, attr)),
      _eb(0), _rb(0), _ib(0)
  {
    QDialogButtonBox *buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok);
    connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));

    if (tableview)
      {
	QTableView *view = new QTableView();
	view->verticalHeader()->hide();
	_view = view;
      }
    else
      {
	QTreeView *view = new QTreeView();
	view->setRootIsDecorated(attr & TableModel::Recursive);
	_view = view;
#if 0
	connect(view, SIGNAL(expanded(const QModelIndex&)),
		this, SLOT(expanded()));
#endif
      }

    if (attr & TableModel::Editable)
      {
	_eb = buttonBox->addButton(tr("Edit"), QDialogButtonBox::ActionRole);
	_eb->setEnabled(false);
	connect(_eb, SIGNAL(clicked()), this, SLOT(edit()));
      }

    if (attr & TableModel::EditRemove)
      {
	_rb = buttonBox->addButton(tr("Remove"), QDialogButtonBox::ActionRole);
	_rb->setEnabled(false);
	connect(_rb, SIGNAL(clicked()), this, SLOT(remove()));
      }

    if (attr & TableModel::EditInsert)
      {
	_ib = buttonBox->addButton(tr("Insert"), QDialogButtonBox::ActionRole);
	connect(_ib, SIGNAL(clicked()), this, SLOT(insert()));
      }

    _view->setModel(_model);

    connect(_view->selectionModel(), SIGNAL(currentChanged(const QModelIndex&, const QModelIndex&)),
	    this, SLOT(currentChanged(const QModelIndex&)));

    QVBoxLayout *layout = new QVBoxLayout;
    layout->addWidget(_view);
    layout->addWidget(buttonBox);

    setLayout(layout);
  }

  TableDialog::~TableDialog()
  {
    delete _model;
  }

  void TableDialog::expanded() const
  {
  }

  void TableDialog::edit() const
  {
    QModelIndex index = editable_index(_view->currentIndex());

    if (index.isValid())
      _view->edit(index);
  }

  void TableDialog::insert() const
  {
    QModelIndex index = _view->currentIndex();

    if (index.isValid())
      {
	_model->insertRow(index.row(), _model->parent(index));
	_view->edit(index);
      }
  }

  void TableDialog::remove() const
  {
    QModelIndex index = _view->currentIndex();

    if (index.isValid())
      _model->removeRow(index.row(), _model->parent(index));
  }

  QModelIndex TableDialog::editable_index(const QModelIndex &index) const
  {
    // auto switch to editable column when using QTreeView
    if (index.isValid() && dynamic_cast<QTreeView*>(_view))
      {
	if (index.column() != TableModel::ColValue &&
	    !(_model->flags(index) & Qt::ItemIsEditable))
	  return _model->index(index.row(), TableModel::ColValue, _model->parent(index));
      }
    return index;
  }

  void TableDialog::currentChanged(const QModelIndex &index) const
  {
    if (_rb)
      _rb->setEnabled(index.isValid());

    if (_eb)
      _eb->setEnabled(_model->flags(editable_index(index)) & Qt::ItemIsEditable);

    // FIXME try to set current index to editable column in row
  }

  void TableDialog::table_dialog(QWidget *parent, const Value &root,
				 const QString &title, TableModel::Attributes attr,
				 bool tableview)
  {
    TableDialog d(parent, root, attr, tableview);
    d.setWindowTitle(title);
    d.resize(d.minimumSizeHint());
    d.exec();
  }

  QSize TableDialog::minimumSizeHint() const
  {
    //    QSize min(0, 640);
    QSize min = QDialog::minimumSizeHint();
    int colcount = _model->columnCount(QModelIndex());

    if (QTreeView *tv = dynamic_cast<QTreeView*>(_view))
      {
	for (int i = 0; i < colcount; i++)
	  {
	    tv->resizeColumnToContents(i);

	    // Leave room for expand
	    if (i == TableModel::ColKey)
	      tv->setColumnWidth(i, tv->columnWidth(i) * 2);

	    min.rwidth() += tv->columnWidth(i);
	  }
      }
    else if (QTableView *tv = dynamic_cast<QTableView*>(_view))
      {
	for (int i = 0; i < colcount; i++)
	  {
	    tv->resizeColumnToContents(i);
	    min.rwidth() += tv->columnWidth(i);
	  }
      }

    return min;
  }

}
