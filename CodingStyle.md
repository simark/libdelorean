C++ Coding style for libdelorean
================================

This document describes the coding style used throughout libdelorean's
C++11 code. Please respect it if you want to contribute.

  * C++ source files have a `.cpp` extension and headers have a `.hpp`
    extension.
  * Indentation: 4 spaces.
  * Do not use `using namespace std`, or `using namespace` at all, both in
    source files and headers. For very long namespaces (e.g. Boost ones), and
    **only in source files**, you may use a namespace alias:

        namespace bfs = boost::filesystem;
        namespace balgo = boost::algorithm;

    Namespace aliases, like any namespace, should contain only lowercase
    characters. For Boost namespace aliases, prefix the alias with `b`.
    Do not alias `std`.
  * `#include` guard syntax: `_FILENAMEWITHOUTEXTENSION_HPP`.
  * Class definition template:

        /**
         * Brief description.
         *
         * Optional longer description.
         *
         * @author Author name
         */
        class SomeClassName :
            public SomeParent,
            public SomeInterface
        {
        public:
            /**
             * Builds some object. Blabla.
             *
             * @param a Parameter description
             * @param b Parameter description
             */
            SomeClassName(int a, int b);

            virtual ~SomeClassName(...);

            /**
             * Does this and that.
             */
            void someMethod();

            /**
             * Does this and that with \p param.
             *
             * @param param Parameter description
             * @returns     Something interesting
             */
            int someOtherMethod(float param) const;

        protected:
            // ...

        private:
            // attribute description
            _someAttribute;
        };

  * Use C++11 initializing list style for everything built, including constructors:

        std::unique_ptr<Hello> myHello {new Hello {3, 4, "lol"}};

    You may break lines within those initializing lists:

        std::unique_ptr<Hello> myHello {
            new Hello {
                3,
                4,
                "lol"
            }
        };

    Use your judgement to make your code as readable as possible.
    Do not, however, break literal strings used for user messages.
  * Try to respect 80 columns as much as possible, visually identing the excess if
    necessary:

        void HistoryFileSink::drawBranchFromIndex(std::size_t parentIndex,
                                                  std::size_t height)
        {
            // ...
        }

    or:

        auto branchNode = this->createBranchNode(parentNode->getSeqNumber(),
                                                 this->getEnd());

  * Use `auto` as much as possible.
  * Make sure to use `auto&` when you don't want to copy a returned reference.
  * Use smart pointers as much as possible, particularly `std::unique_ptr` when it's
    semantically correct to do so.
  * Do not use `std::move(ptr)` when returning a `std::unique_ptr`.
  * Use iterators as much as possible, even with `std::vector`.
  * Use range-based for loop as much as possible.
  * Comment a lot, but not when it's obvious.
  * Access attributes using their name and methods using `this`:

        _myAttribute = 23;
        this->doSomething(5, _anotherAttribute);
  * Use references as much as possible. Use const references for parameters as much
    as possible. Almost no "loose" pointers should be in your code.
  * Use `const` methods as much as possible.
  * Do not use C-style casting. Use `static_cast`, `reinterepret_cast` and, when
    it's really needed, `dynamic_cast` instead.
  * Use `std::size_t` for any index or size.
  * When writing a new concrete class, write a CppUnit test for it.
  * Write Doxygen comments in **headers** _at least_ for all public methods.
  * Do not use `NULL`; use `nullptr`.
  * Prefix abstract classes names with `Abstract` and interfaces with `I`.
  * Interfaces should not implement any method or inherit parents that implement
    any method (use abstract classes for that).
  * Use `typedef` as much as possible. Class-scoped type definitions are good,
    public, protected or private:

        class Something
        {
        public:
            /// Shared pointer to something
            typedef std::shared_ptr<Something> SP;

            /// Unique pointer to something
            typedef std::unique_ptr<Something> UP;

        ...

  * Strip all trailing whitespaces before committing.
