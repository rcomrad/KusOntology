#--------------------------------------------------------------------------------

macro(get_sources SOURCE_LIST DIR)
    set(FOLDER_NAMES code core domain file_data)
    add_sources(${SOURCE_LIST} "${DIR}/source" "${FOLDER_NAMES}")
endmacro()

#--------------------------------------------------------------------------------
