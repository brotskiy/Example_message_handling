# Adds applock source files.

defineTest(makeApplock) {
    APPLOCK_REPO_PATH = $$1

    isEmpty(APPLOCK_REPO_PATH) {
        message("Can't find applock repository!")

        return(false)
    }

    INCLUDEPATH += $${APPLOCK_REPO_PATH}
    HEADERS += $$files($${APPLOCK_REPO_PATH}/*.h)
    SOURCES += $$files($${APPLOCK_REPO_PATH}/*.cpp)

    isEmpty(HEADERS)|isEmpty(SOURCES) {
        message("Applock repository at $${APPLOCK_REPO_PATH} is empty!")
    }

    export(INCLUDEPATH)
    export(HEADERS)
    export(SOURCES)

    message("Applock repository at $${APPLOCK_REPO_PATH} is successfully added!")

    return(true)
}
