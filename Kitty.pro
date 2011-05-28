TEMPLATE = subdirs
SUBDIRS  = lib src
CONFIG += ordered warn_on

LANGUAGES = pl

include(src/translations.pri)
