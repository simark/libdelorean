#include <iostream>
#include <cppunit/CompilerOutputter.h>
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/ui/text/TestRunner.h>

int main()
{
    // top level test suite from the registry
    auto& registry = CppUnit::TestFactoryRegistry::getRegistry();
    auto suite = registry.makeTest();

    // add test suite to new text UI runner
    CppUnit::TextUi::TestRunner runner;
    runner.addTest(suite);

    // change the default outputter to a compiler error format outputter
    auto compilerOutput = new CppUnit::CompilerOutputter(&runner.result(),
                                                         std::cerr);
    runner.setOutputter(compilerOutput);

    // run all tests
    bool success = runner.run();

    return success ? 0 : 1;
}
