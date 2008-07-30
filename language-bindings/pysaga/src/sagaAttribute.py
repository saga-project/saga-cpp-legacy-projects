class Attribute(object):
    pass

package saga.attributes
{
  interface attributes
  {
    // setter / getters
    set_attribute         (in  string        key,
                           in  string        value);
    get_attribute         (in  string        key,
                           out string        value);
    set_vector_attribute  (in  string        key,
                           in  array<string> values);
    get_vector_attribute  (in  string        key,
                           out array<string> values);
    remove_attribute      (in  string        key);
    // inspection methods
    list_attributes       (out array<string> keys);
    find_attributes       (in array<string>  pattern,
                            out array<string> keys);
    attribute_exists       (in  string        key,
                            out bool          test);
    attribute_is_readonly  (in  string        key,
                            out bool          test);
    attribute_is_writable  (in  string        key,
                            out bool          test);
    attribute_is_removable (in  string        key,
                            out bool          test);
    attribute_is_vector    (in  string        key,
                            out bool          test);
  }
}
