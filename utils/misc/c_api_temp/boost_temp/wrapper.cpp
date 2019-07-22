#include <boost/python.hpp>

BOOST_PYTHON_MODULE(hello_cpp) {
  using namespace boost::python;
  def("greet", greet);
}
