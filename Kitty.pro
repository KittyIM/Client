TEMPLATE = subdirs
SUBDIRS  = src\\3rdparty\\hunspell src
CONFIG += ordered warn_on

LANGUAGES = pl

include(src/translations.pri)
