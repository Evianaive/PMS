# For help with CMake, see: $S/cmake/Help/H_Help.cmake
# Please copy this header in all new CMakeLists.txt files.

project( support_icons )

add_subdirectory( archivemaker )

set( index_file SVGIcons.index )
set( resource_files
    IconMapping
)

set( svg_file_list "${H_GLOBBED_DEPS_PATH}/svg_file_list.txt" )
h_glob_target(
    TARGET_NAME glob_svg_icons
    LIST_FILE ${svg_file_list}
    EXTENSIONS svg
    COMMENT "Globbing .svg files"
)

set( non_svg_file_list "${H_GLOBBED_DEPS_PATH}/non_svg_file_list.txt" )
h_glob_target(
    TARGET_NAME glob_non_svg_icons
    LIST_FILE ${non_svg_file_list}
    FOLDERS Pics
    EXTENSIONS pic png svg
    COMMENT "Globbing .pic and .png files"
)

add_custom_command(
    OUTPUT ${H_CONFIG_ICONS_INSTALL_PATH}/${index_file}
    COMMAND ${CMAKE_COMMAND}
        -E env PATH="${H_MINIMAL_PATH}"
        $<TARGET_FILE:svgarchivemaker>
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_CURRENT_BINARY_DIR}
    COMMAND ${CMAKE_COMMAND} -E copy
        ${index_file}
        ${H_CONFIG_ICONS_INSTALL_PATH}/${index_file}
    DEPENDS
        svgarchivemaker
	${H_BASE_PLUGIN_DEPS}
        glob_svg_icons
        ${svg_file_list}
)

# Copy the globbed .pic and .png files to HFS.
set( pic_stamp_file ${H_GLOBBED_DEPS_PATH}/copied_pic_files.stamp )
add_custom_command(
    OUTPUT ${pic_stamp_file}
    DEPENDS ${BT_UTILITY_SCRIPTS}/CopyFiles.py ${non_svg_file_list} glob_non_svg_icons
    COMMAND ${BT_PYTHON} ${BT_UTILITY_SCRIPTS}/CopyFiles.py
        ${CMAKE_CURRENT_SOURCE_DIR}/Pics
        ${H_CONFIG_ICONS_INSTALL_PATH}
        -l ${non_svg_file_list}
    COMMAND ${CMAKE_COMMAND} -E touch ${pic_stamp_file}
    COMMENT "Installing png and pic icons."
)

h_copy_command(
    DESTINATION ${H_SUPPORT_CONFIG_INSTALL_PATH}
    SOURCE ${CMAKE_CURRENT_SOURCE_DIR}
    FILES ${resource_files}
    OUTPUT output_resource_files
)

h_custom_target(
    ${PROJECT_NAME}
    ALL
    DEPENDS
        ${H_CONFIG_ICONS_INSTALL_PATH}/${index_file}
        ${output_resource_files}
        ${pic_stamp_file}
)
