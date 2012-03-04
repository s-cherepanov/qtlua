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


#ifndef QTLUAVALUEBASE_HH_
#define QTLUAVALUEBASE_HH_

#include <QHash>
#include <QList>
#include <QPointer>
#include <QVariant>

#include "qtluastring.hh"
#include "qtluaref.hh"

struct lua_State;

namespace QtLua {

class Value;
class ValueRef;
class State;
class UserData;
class TableIterator;
class Iterator;

/** @internal */
uint qHash(const Value &lv);

  /**
   * @short Lua values wrapper class
   * @header QtLua/Value
   * @module {Base}
   *
   * This class exposes a lua value to C++ code. It provides
   * conversion functions, cast operators, access operators and
   * standard C++ iterators.
   *
   * Each @ref QtLua::Value object store its associated lua value in
   * the lua interpreter state registry table.
   * 
   * @xsee{Qt/Lua types conversion}
   * @see Iterator
   * @see iterator
   * @see const_iterator
   */

class ValueBase
{
  friend class State;
  friend class UserData;
  friend class TableIterator;
  friend class Value;
  friend class ValueRef;

  inline ValueBase(const State *ls);

  inline virtual ~ValueBase();

  /**
   * @internal 
   * @short Value iterator base class
   */
  class iterator_
  {
    friend class ValueBase;
    friend class UserData;

  public:
    typedef std::forward_iterator_tag iterator_category;

    /** @internal */
    inline iterator_(const Ref<Iterator> &i);
    /** Create a non initialized iterator */
    inline iterator_();
    inline iterator_ & operator++();
    inline iterator_ operator++(int);
    inline bool operator==(const iterator_ &i) const;
    inline bool operator!=(const iterator_ &i) const;
    /** Get current entry table key */
    inline Value key() const;

  private:
    Ref<Iterator> _i;
  };

public:

  /**
   * @short Value iterator class.
   * @header QtLua/Value
   *
   * This iterator class allows iteration through table like lua values
   * directly from C++ code. Assignation of @ref ValueRef object
   * returned by @ref value @strong modify traversed container.
   *
   * @see const_iterator
   */
  struct iterator : public iterator_
  {
    /** @internal */
    inline iterator(const Ref<Iterator> &i);
    /** Create an uninitialized @ref iterator. */
    inline iterator();
    /** Get modifiable reference to current entry value. */
    inline ValueRef value();
    /** @see value */
    inline ValueRef operator* ();
  };

  /**
   * @short Value const iterator class.
   * @header QtLua/Value
   *
   * This iterator class allow iteration through table like lua values
   * directly from C++ code. Modification of @ref Value object
   * returned by @ref value @strong{doesn't modify} traversed
   * container.
   *
   * @see iterator
   */
  struct const_iterator : public iterator_
  {
    /** @internal */
    inline const_iterator(const Ref<Iterator> &i);
    /** Create from non const iterator */
    inline const_iterator(const iterator &i);
    /** Create a non uninitialized @ref const_iterator. */
    inline const_iterator();
    /** Get current entry value. */
    inline Value value() const;
    /** @see value */
    inline Value operator* () const;
  };
 
  /** Specify lua value types. This is the same as @tt LUA_T* macros defined in lua headers */
  enum ValueType
    {
      TNone	= -1,		//< No type
      TNil	= 0,		//< Nil value
      TBool	= 1,		//< Boolean value
      TNumber	= 3,		//< Number value
      TString	= 4,		//< String value
      TTable	= 5,		//< Lua table value
      TFunction	= 6,		//< Lua function value
      TUserData	= 7,		//< Lua userdata value
    };

  /**
   * Specify lua operations performed on lua values.
   * @see support @see UserData::meta_operation @see UserData::support
   */
  enum Operation
    {
      OpAdd       = 0x0001,     //< Lua add binary operator @tt +
      OpSub       = 0x0002,     //< Lua subtract binary operator @tt -
      OpMul       = 0x0004,     //< Lua multiply binary operator @tt *
      OpDiv       = 0x0008,     //< Lua divied binary operator @tt /
      OpMod       = 0x0010,     //< Lua modulo binary operator @tt %
      OpPow       = 0x0020,     //< Lua power binary operator @tt ^
      OpUnm       = 0x0040,     //< Lua negative unary operator @tt -
      OpConcat    = 0x0080,     //< Lua concatenation binary operator @tt ..
      OpLen       = 0x0100,     //< Lua length unary operator @tt #
      OpEq        = 0x0200,     //< Lua equal binary operator @tt ==
      OpLt        = 0x0400,     //< Lua less than binary operator @tt <
      OpLe        = 0x0800,     //< Lua less than or equal binary operator @tt <=

      OpIndex     = 0x1000, 	//< Table index operation
      OpNewindex  = 0x2000,     //< Table newindex operation
      OpCall      = 0x4000,     //< Function call operation
      OpIterate   = 0x8000,     //< Iteration operation

      OpAll       = 0xffff,     //< All operations mask
    };

  Q_DECLARE_FLAGS(Operations, Operation);

  /**
   * @short List of Value objects used for arguments and return values.
   *
   * List of @ref Value objects used for lua functions arguments and return values.
   */
  struct List : public QList<Value>
  {
    inline List();
    inline List(const List &vl);

    /** Create value list with one @ref Value object */
    inline List(const Value &v1);

    /** Create value list with @ref Value objects. @multiple */
    inline List(const Value &v1, const Value &v2);
    inline List(const Value &v1, const Value &v2, const Value &v3);
    inline List(const Value &v1, const Value &v2, const Value &v3, const Value &v4);
    inline List(const Value &v1, const Value &v2, const Value &v3, const Value &v4, const Value &v5);
    inline List(const Value &v1, const Value &v2, const Value &v3, const Value &v4, const Value &v5, const Value &v6);
    /** Create value list from @ref QList of @ref Value objects */
    inline List(const QList<Value> &list);

    /** Create value list from @ref QList content */
    template <typename X>
    inline List(const State *ls, const QList<X> &list);

    /** Create value list from @ref QList content */
    template <typename X>
    inline List(const State *ls, const typename QList<X>::const_iterator &begin,
		const typename QList<X>::const_iterator &end);

    /** return a @ref QList with all elements converted from lua values */
    template <typename X>
    QList<X> to_qlist() const;
    /** return a @ref QList with elements converted from lua values */
    template <typename X>
    static QList<X> to_qlist(const const_iterator &begin, const const_iterator &end);

    /** return a lua table containing all values from list */
    inline Value to_table(const State *ls) const;
    /** return a lua table containing values from list */
    static inline Value to_table(const State *ls, const const_iterator &begin, const const_iterator &end);
  };


  /**
   * @showcontent
   *
   * Boolean type used for Value constructor.
   *
   * The native C++ @tt bool type is not used here due to implicit
   * cast between @tt bool and pointers which prevent proper
   * constructor overloading.
   */
  enum Bool
    {
      False = 0,
      True = 1
    };

  /** Call operation on a lua userdata or lua function value. @multiple */
  List call (const List &args) const;
  inline List operator() () const;
  inline List operator() (const Value &arg1) const;
  inline List operator() (const Value &arg1, const Value &arg2) const;
  inline List operator() (const Value &arg1, const Value &arg2, const Value &arg3) const;
  inline List operator() (const Value &arg1, const Value &arg2, const Value &arg3, const Value &arg4) const;
  inline List operator() (const Value &arg1, const Value &arg2, const Value &arg3, const Value &arg4, const Value &arg5) const;
  inline List operator() (const Value &arg1, const Value &arg2, const Value &arg3, const Value &arg4, const Value &arg5, const Value &arg6) const;

  /** Get an @ref iterator to traverse a lua userdata or lua table value. @multiple */
  inline iterator begin();
  inline iterator end();

  /** Get a @ref const_iterator to traverse a lua userdata or lua table value. @multiple */
  inline const_iterator begin() const;
  inline const_iterator end() const;

  /** Return an @ref Iterator object suitable to iterate over lua value.
      This works for lua tables and @ref UserData objects. */
  Ref<Iterator> new_iterator() const;

  inline operator Value() const;

  /** Convert a lua number value to a @tt double.
      Throw exception if conversion fails. @multiple */
  double to_number() const;
  inline operator double () const;
  inline operator float () const;

  /** Convert a lua number value to an integer.
      Throw exception if conversion fails. @multiple */
  inline int to_integer() const;
  inline operator int () const;
  inline operator unsigned int () const;

  /** Convert a lua value to a boolean.
      Throw exception if conversion fails. @multiple */
  Bool to_boolean() const;
  inline operator Bool () const;

  /** Convert a lua string value to a @ref String object.
      Throw exception if conversion fails. @multiple */
  String to_string() const;
  inline QString to_qstring() const;
  inline operator String () const;
  inline operator QString () const;

  /** Convert a lua string value to a @tt C string.
      Throw exception if conversion fails. */
  const char * to_cstring() const;

  /** Convert any type to a string representation suitable for pretty
      printing. Never throw. */
  String to_string_p(bool quote_string = true) const;

  /**
   * Create a @ref QList with elements from lua table. Table keys are
   * searched from 1.
   * @xsee{Qt/Lua types conversion}
   * @multiple
   */
  template <typename X>
  QList<X> to_qlist() const;
  template <typename X>
  operator QList<X> () const;

  /**
   * Create a @ref QVector with elements from lua table. Table keys are
   * searched from 1.
   * @xsee{Qt/Lua types conversion}
   * @multiple
   */
  template <typename X>
  QVector<X> to_qvector() const;
  template <typename X>
  operator QVector<X> () const;

  /**
   * Create a @ref QHash with elements from lua table.
   * @xsee{Qt/Lua types conversion}
   * @multiple
   */
  template <typename Key, typename Val>
  QHash<Key, Val> to_qhash() const;
  template <typename Key, typename Val>
  operator QHash<Key, Val> () const;

  /**
   * Create a @ref QMap with elements from lua table.
   * @xsee{Qt/Lua types conversion}
   * @multiple
   */
  template <typename Key, typename Val>
  QMap<Key, Val> to_qmap() const;
  template <typename Key, typename Val>
  operator QMap<Key, Val> () const;

  /**
   * Convert a lua value to a @ref QObject pointer.
   * Throw exception if conversion or cast fails.
   * @see to_qobject_cast
   */
  QObject *to_qobject() const;

  /**
   * Convert a lua value to a @ref QObject subclass pointer. Subclass
   * must have the @tt Q_OBJECT macro in it's definition.
   * Throw exception if conversion or cast fails.
   * @see to_qobject
   */
  template <class X>
  inline X *to_qobject_cast() const;

  /**
   * Convert a lua value to a @ref Ref pointer to an @ref UserData.
   * Throw exception if conversion fails.
   * @see to_userdata_null
   * @see to_userdata_cast
   */
  Ref<UserData> to_userdata() const;

  /**
   * Convert a lua value to a @ref Ref pointer to an @ref UserData.
   * @return a null @ref Ref if conversion fails.
   * @see to_userdata
   * @see to_userdata_cast
   */
  Ref<UserData> to_userdata_null() const;

  /**
   * Convert a lua value to a @ref Ref pointer to an @ref UserData and
   * dynamic cast to given @ref Ref pointer to requested type.
   * Throw exception if conversion or cast fails.
   * @see to_userdata
   * @see to_userdata_cast
   */
  template <class X>
  inline Ref<X> to_userdata_cast() const;

  /** @see to_userdata_cast */
  template <class X>
  inline operator Ref<X> () const;

  /**
   * Convert a lua value to a @ref QVariant.
   * @xsee {Qt/Lua types conversion} @multiple
   */
  QVariant to_qvariant() const; 
  inline operator QVariant () const;

  /** Index operation on a lua userdata or lua table value. @multiple */
  Value at(const Value &key) const;

  template <typename T>
  inline Value at(const T &key) const;

  /** Index operation on a lua userdata or lua table value. The @ref
      at function is prefered for read access.  @multiple */
#if 1
  inline Value operator[] (const Value &key) const;

  template <typename T>
  inline Value operator[] (const T &key) const;
#endif

  inline ValueRef operator[] (const Value &key);

  template <typename T>
  inline ValueRef operator[] (const T &key);

  /** Check if the value is @tt nil */
  inline bool is_nil() const;

  /** Dump the bytecode for a function object */
  QByteArray to_bytecode() const;

  /** Get lua value type. */
  ValueType type() const;

  /** Get value raw lua type name. */
  String type_name() const;

  /** Get value raw lua type name. */
  static String type_name(ValueType t);

  /** Get value type name, if the value is a @ref UserData, the type
      name is extracted using the @ref UserData::get_type_name function. */
  String type_name_u() const;

  /** Return the lua len of tables and strings. Return the result of
      the @ref OpLen operation on @ref UserData objects or 0 if not
      supported. */
  int len() const;

  /** Check given operation support. @see UserData::support */
  bool support(Operation c) const;

  /** Compare lua values. @multiple */
  bool operator<(const Value &lv) const;
  bool operator==(const Value &lv) const;

  /** Compare lua values with given string. */
  bool operator==(const String &str) const;
  /** Compare lua values with given C string. */
  bool operator==(const char *str) const;
  /** Compare lua values with given number. */
  bool operator==(double n) const;

  /** Get associated lua state. */
  inline State * get_state() const;

  /**
   * Connect a @ref QObject signal to a lua value. The value will be
   * called when the signal is emited.
   * @see disconnect
   * @see QObject::connect
   * @xsee{QObject wrapping}
   */
  bool connect(QObject *obj, const char *signal);

  /**
   * Disconnect a @ref QObject signal from a lua value.
   * @see connect
   * @see QObject::disconnect
   * @xsee{QObject wrapping}
   */
  bool disconnect(QObject *obj, const char *signal);

protected:
  template <typename HashContainer>
  HashContainer to_hash() const;

  template <typename ListContainer>
  ListContainer to_list() const;

  virtual void push_value() const = 0;
  virtual Value value() const = 0;

  static String to_string_p(lua_State *st, int index, bool quote_string);

  static uint qHash(lua_State *st, int index);

  void convert_error(ValueType type) const;
  void check_state() const;

  QPointer<State> _st;
  static double _id_counter;
};

}

Q_DECLARE_OPERATORS_FOR_FLAGS(QtLua::ValueBase::Operations);

#endif

