defineReplace(prependAll) {
  for(a,$$1):result += $$2$${a}$$3
  return($$result)
}

defineReplace(targetPath) {
  return($$replace(1, /, $$QMAKE_DIR_SEP))
}

LANGUAGES_UP = $$LANGUAGES untranslated

TRANSLATIONS = $$prependAll(LANGUAGES, ../translations/kitty_,.ts)
TRANSLATIONS_UP = $$prependAll(LANGUAGES_UP, ../translations/kitty_,.ts)

LUPDATE = $$targetPath($$[QT_INSTALL_BINS]/lupdate) $$PWD -locations absolute -no-ui-lines -no-sort -no-obsolete
system($$LUPDATE -ts $$TRANSLATIONS_UP)

LRELEASE = $$targetPath($$[QT_INSTALL_BINS]/lrelease)
system(LRELEASE $$TRANSLATIONS -silent && move $$targetPath(../translations/*.qm) $$targetPath(../bin/data/translations/))
