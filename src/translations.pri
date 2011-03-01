!isEmpty(PRI_INCLUDED):error("pri already included")
PRI_INCLUDED = 1

defineReplace(prependAll) {
  for(a,$$1):result += $$2$${a}$$3
  return($$result)
}

defineReplace(targetPath) {
  return($$replace(1, /, $$QMAKE_DIR_SEP))
}

TRANSLATIONS = $$prependAll(LANGUAGES, ../translations/kitty_,.ts)

LUPDATE = $$targetPath($$[QT_INSTALL_BINS]/lupdate) $$PWD -locations absolute -no-ui-lines -no-sort -no-obsolete -silent
system($$LUPDATE -ts $$TRANSLATIONS)

LRELEASE = $$targetPath($$[QT_INSTALL_BINS]/lrelease)
system(LRELEASE $$TRANSLATIONS -silent && move $$targetPath(../translations/*.qm) $$targetPath(../bin/data/translations/))
