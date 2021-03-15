![build](https://github.com/GwenaelCasaccio/smalltalk/actions/workflows/smalltalk.yml/badge.svg)

# GST

GST is an implementation of the Smalltalk language as described. 

# Smalltalk
The Smalltalk programming language is an object oriented
programming language.  This means, for one thing, that when
programming you are thinking of not only the data that an object
contains, but also of the operations available on that object.  The
object's data representation capabilities and the operations available
on the object are "inseparable"; the set of things that you can do
with an object is defined precisely by the set of operations, which
Smalltalk calls "methods", that are available for that object.  You
cannot even examine the contents of an object from the outside.  To an
outsider, the object is a black box that has some state and some
operations available, but that's all you know.

In the Smalltalk language, everything is an object.  This includes
numbers, executable procedures (methods), stack frames (called method
contexts or block contexts), etc.  Each object is an "instance" of a
"class".  A class can be thought of as a datatype and the set of
functions that operate on that datatype.  An instance is a particular
variable of that datatype. When you want to perform an operation on an
object, you send it a "message", and the object performs an operation
that corresponds to that message.

