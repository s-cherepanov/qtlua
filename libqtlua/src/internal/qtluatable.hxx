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

    Copyright (C) 2008, Alexandre Becoulet <alexandre.becoulet@free.fr>

*/


#ifndef QTLUA_TABLE_HXX_
#define QTLUA_TABLE_HXX_

namespace QtLua {

  const Value & Table::get_key(int n) const
  {
    return _entries[n]._key;
  }

  void Table::set_key(int n, const Value &key)
  {
    _entries[n]._key = key;
  }

  bool Table::is_table(int n) const
  {
    return _entries[n]._table != 0;
  }

  size_t Table::count() const
  {
    return _entries.count();
  }

  Table::Entry::Entry(const Value &key)
    : _key(key),
      _table(0),
      _table_chk(false)
  {
  }

  bool Table::Entry::operator<(const Entry &e) const
  {
    return _key < e._key;
  }

}

#endif

