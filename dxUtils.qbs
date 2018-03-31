import qbs 1.0
import qbs.TextFile
import qbs.FileInfo


CppApplication {
    type: "staticlibrary"
    name: "dxUtils"

    cpp.cxxLanguageVersion: "c++11"

    files: [
        "src/*",
        "include/**"
    ]

    cpp.includePaths: [
        "include/"
    ]

    cpp.defines: [
    ]

    cpp.commonCompilerFlags: [

    ]

    cpp.cxxFlags: [

    ]


    cpp.dynamicLibraries: [
        "pthread",
        "glog"
    ]


    cpp.linkerFlags: [
        "-Wall",
    ]

    cpp.libraryPaths: [
    ]

    cpp.staticLibraries: [
    ]

    Export {

        Depends { name: "cpp" }

        cpp.includePaths: [
            "include/",
        ]
    }

    Group {     // Properties for the produced executable
        fileTagsFilter: product.type
        qbs.install: false
    }
}
