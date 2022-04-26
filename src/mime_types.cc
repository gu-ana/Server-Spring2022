#include "mime_types.h"

namespace server {
namespace mime_types {

struct mapping
{
  const char* extension;
  const char* mime_type;
} mappings[] =
{
  { ".gif",  "image/gif"   },
  { ".htm",  "text/html"   },
  { ".html", "text/html"   },
  { ".jpg",  "image/jpeg"  },
  { ".png",  "image/png"   },
  { ".pdf",  "application/pdf"},
  { ".zip",  "application/zip"}

};

std::string extension_to_type(const std::string& extension)
{
  for (mapping m: mappings)
  {
    if (m.extension == extension)
    {
      return m.mime_type;
    }
  }

  return "text/plain";
}
} //name space mime_types
}  //namespace server
