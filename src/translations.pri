defineReplace(prependAll) {
  for(a,$$1):result += $$2$${a}$$3
  return($$result)
}

defineReplace(targetPath) {
  return($$replace(1, /, $$QMAKE_DIR_SEP))
}

TRANSLATIONS = $$prependAll(LANGUAGES, ../translations/kitty_,.ts)

LUPDATE = $$targetPath($$[QT_INSTALL_BINS]/lupdate) -locations absolute -no-ui-lines -no-sort -silent
system($$LUPDATE $$PWD -ts $$TRANSLATIONS)
