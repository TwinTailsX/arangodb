////////////////////////////////////////////////////////////////////////////////
/// @brief auto-generated file generated from mimetypes.dat
////////////////////////////////////////////////////////////////////////////////

#include "Basics/Common.h"
#include "./lib/Basics/voc-mimetypes.h"

////////////////////////////////////////////////////////////////////////////////
/// @brief initialize mimetypes
////////////////////////////////////////////////////////////////////////////////

void TRI_InitializeEntriesMimetypes () {
  TRI_RegisterMimetype("gif", "image/gif", false);
  TRI_RegisterMimetype("jpg", "image/jpg", false);
  TRI_RegisterMimetype("png", "image/png", false);
  TRI_RegisterMimetype("tiff", "image/tiff", false);
  TRI_RegisterMimetype("ico", "image/x-icon", false);
  TRI_RegisterMimetype("css", "text/css", true);
  TRI_RegisterMimetype("js", "text/javascript", true);
  TRI_RegisterMimetype("json", "application/json", true);
  TRI_RegisterMimetype("html", "text/html", true);
  TRI_RegisterMimetype("htm", "text/html", true);
  TRI_RegisterMimetype("pdf", "application/pdf", false);
  TRI_RegisterMimetype("ps", "application/postscript", false);
  TRI_RegisterMimetype("txt", "text/plain", true);
  TRI_RegisterMimetype("text", "text/plain", true);
  TRI_RegisterMimetype("xml", "application/xml", true);
  TRI_RegisterMimetype("dtd", "application/xml-dtd", true);
  TRI_RegisterMimetype("svg", "image/svg+xml", true);
  TRI_RegisterMimetype("ttf", "application/x-font-ttf", false);
  TRI_RegisterMimetype("otf", "application/x-font-opentype", false);
  TRI_RegisterMimetype("woff", "application/font-woff", false);
  TRI_RegisterMimetype("eot", "application/vnd.ms-fontobject", false);
  TRI_RegisterMimetype("bz2", "application/x-bzip2", false);
  TRI_RegisterMimetype("gz", "application/x-gzip", false);
  TRI_RegisterMimetype("tgz", "application/x-tar", false);
  TRI_RegisterMimetype("zip", "application/x-compressed-zip", false);
  TRI_RegisterMimetype("doc", "application/msword", false);
  TRI_RegisterMimetype("docx", "application/vnd.openxmlformats-officedocument.wordprocessingml.document", false);
  TRI_RegisterMimetype("dotx", "application/vnd.openxmlformats-officedocument.wordprocessingml.template", false);
  TRI_RegisterMimetype("potx", "application/vnd.openxmlformats-officedocument.presentationml.template", false);
  TRI_RegisterMimetype("ppsx", "application/vnd.openxmlformats-officedocument.presentationml.slideshow", false);
  TRI_RegisterMimetype("ppt", "application/vnd.ms-powerpoint", false);
  TRI_RegisterMimetype("pptx", "application/vnd.openxmlformats-officedocument.presentationml.presentation", false);
  TRI_RegisterMimetype("xls", "application/vnd.ms-excel", false);
  TRI_RegisterMimetype("xlsb", "application/vnd.ms-excel.sheet.binary.macroEnabled.12", false);
  TRI_RegisterMimetype("xlsx", "application/vnd.openxmlformats-officedocument.spreadsheetml.sheet", false);
  TRI_RegisterMimetype("xltx", "application/vnd.openxmlformats-officedocument.spreadsheetml.template", false);
  TRI_RegisterMimetype("swf", "application/x-shockwave-flash", false);
}
