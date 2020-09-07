ASITUS_THIRDPARTY_PRODUCT("OCCT" "" "Standard.hxx" "TKernel")

message (STATUS "... OCCT Include dirs: ${3RDPARTY_OCCT_INCLUDE_DIR}")
message (STATUS "... OCCT Library dirs: ${3RDPARTY_OCCT_LIBRARY_DIR}")
message (STATUS "... OCCT Binary  dirs: ${3RDPARTY_OCCT_DLL_DIR}")

string (REPLACE lib libd 3RDPARTY_OCCT_LIBRARY_DIR_DEBUG ${3RDPARTY_OCCT_LIBRARY_DIR})
if (3RDPARTY_OCCT_LIBRARY_DIR_DEBUG AND EXISTS "${3RDPARTY_OCCT_LIBRARY_DIR_DEBUG}")
  if (WIN32)
    if (NOT EXISTS "${3RDPARTY_OCCT_LIBRARY_DIR_DEBUG}/TKernel.lib")
      set (3RDPARTY_OCCT_LIBRARY_DIR_DEBUG "" CACHE INTERNAL FORCE)
    endif()
  else()
    if (NOT EXISTS "${3RDPARTY_OCCT_LIBRARY_DIR_DEBUG}/libTKernel.so")
      set (3RDPARTY_OCCT_LIBRARY_DIR_DEBUG "" CACHE INTERNAL FORCE)
    endif()
  endif()
endif()

if (WIN32)
  string (REPLACE bin bind 3RDPARTY_OCCT_DLL_DIR_DEBUG ${3RDPARTY_OCCT_DLL_DIR})
  if (3RDPARTY_OCCT_DLL_DIR_DEBUG AND EXISTS "${3RDPARTY_OCCT_DLL_DIR_DEBUG}")
    if (NOT EXISTS "${3RDPARTY_OCCT_DLL_DIR_DEBUG}/TKernel.dll")
      set (3RDPARTY_OCCT_DLL_DIR_DEBUG "" CACHE INTERNAL FORCE)
    endif()
  endif()
endif()

message (STATUS "... OCCT Debug Library dirs: ${3RDPARTY_OCCT_LIBRARY_DIR_DEBUG}")
message (STATUS "... OCCT Debug Binary  dirs: ${3RDPARTY_OCCT_DLL_DIR_DEBUG}")

#--------------------------------------------------------------------------
# Installation
#--------------------------------------------------------------------------

if (WIN32)
  message (STATUS "... OCCT binaries: ${3RDPARTY_OCCT_DLL_DIR}")

  install (FILES ${3RDPARTY_OCCT_DLL_DIR}/TKBin.dll       DESTINATION bin)
  install (FILES ${3RDPARTY_OCCT_DLL_DIR}/TKBinL.dll      DESTINATION bin)
  install (FILES ${3RDPARTY_OCCT_DLL_DIR}/TKBO.dll        DESTINATION bin)
  install (FILES ${3RDPARTY_OCCT_DLL_DIR}/TKBool.dll      DESTINATION bin)
  install (FILES ${3RDPARTY_OCCT_DLL_DIR}/TKBRep.dll      DESTINATION bin)
  install (FILES ${3RDPARTY_OCCT_DLL_DIR}/TKCAF.dll       DESTINATION bin)
  install (FILES ${3RDPARTY_OCCT_DLL_DIR}/TKCDF.dll       DESTINATION bin)
  install (FILES ${3RDPARTY_OCCT_DLL_DIR}/TKernel.dll     DESTINATION bin)
  install (FILES ${3RDPARTY_OCCT_DLL_DIR}/TKG2d.dll       DESTINATION bin)
  install (FILES ${3RDPARTY_OCCT_DLL_DIR}/TKG3d.dll       DESTINATION bin)
  install (FILES ${3RDPARTY_OCCT_DLL_DIR}/TKGeomAlgo.dll  DESTINATION bin)
  install (FILES ${3RDPARTY_OCCT_DLL_DIR}/TKGeomBase.dll  DESTINATION bin)
  install (FILES ${3RDPARTY_OCCT_DLL_DIR}/TKIGES.dll      DESTINATION bin)
  install (FILES ${3RDPARTY_OCCT_DLL_DIR}/TKLCAF.dll      DESTINATION bin)
  install (FILES ${3RDPARTY_OCCT_DLL_DIR}/TKMath.dll      DESTINATION bin)
  install (FILES ${3RDPARTY_OCCT_DLL_DIR}/TKMesh.dll      DESTINATION bin)
  install (FILES ${3RDPARTY_OCCT_DLL_DIR}/TKOffset.dll    DESTINATION bin)
  install (FILES ${3RDPARTY_OCCT_DLL_DIR}/TKPrim.dll      DESTINATION bin)
  install (FILES ${3RDPARTY_OCCT_DLL_DIR}/TKShHealing.dll DESTINATION bin)
  install (FILES ${3RDPARTY_OCCT_DLL_DIR}/TKSTEP.dll      DESTINATION bin)
  install (FILES ${3RDPARTY_OCCT_DLL_DIR}/TKSTEP209.dll   DESTINATION bin)
  install (FILES ${3RDPARTY_OCCT_DLL_DIR}/TKSTEPAttr.dll  DESTINATION bin)
  install (FILES ${3RDPARTY_OCCT_DLL_DIR}/TKSTEPBase.dll  DESTINATION bin)
  install (FILES ${3RDPARTY_OCCT_DLL_DIR}/TKTopAlgo.dll   DESTINATION bin)
  install (FILES ${3RDPARTY_OCCT_DLL_DIR}/TKXSBase.dll    DESTINATION bin)
  install (FILES ${3RDPARTY_OCCT_DLL_DIR}/TKHLR.dll       DESTINATION bin)
  install (FILES ${3RDPARTY_OCCT_DLL_DIR}/TKFillet.dll    DESTINATION bin)
  install (FILES ${3RDPARTY_OCCT_DLL_DIR}/TKSTL.dll       DESTINATION bin)
else()
  message (STATUS "... OCCT libraries: ${3RDPARTY_OCCT_LIBRARY_DIR}")

  install (FILES ${3RDPARTY_OCCT_LIBRARY_DIR}/libTKBin.so       DESTINATION bin)
  install (FILES ${3RDPARTY_OCCT_LIBRARY_DIR}/libTKBinL.so      DESTINATION bin)
  install (FILES ${3RDPARTY_OCCT_LIBRARY_DIR}/libTKBO.so        DESTINATION bin)
  install (FILES ${3RDPARTY_OCCT_LIBRARY_DIR}/libTKBool.so      DESTINATION bin)
  install (FILES ${3RDPARTY_OCCT_LIBRARY_DIR}/libTKBRep.so      DESTINATION bin)
  install (FILES ${3RDPARTY_OCCT_LIBRARY_DIR}/libTKCAF.so       DESTINATION bin)
  install (FILES ${3RDPARTY_OCCT_LIBRARY_DIR}/libTKCDF.so       DESTINATION bin)
  install (FILES ${3RDPARTY_OCCT_LIBRARY_DIR}/libTKernel.so     DESTINATION bin)
  install (FILES ${3RDPARTY_OCCT_LIBRARY_DIR}/libTKG2d.so       DESTINATION bin)
  install (FILES ${3RDPARTY_OCCT_LIBRARY_DIR}/libTKG3d.so       DESTINATION bin)
  install (FILES ${3RDPARTY_OCCT_LIBRARY_DIR}/libTKGeomAlgo.so  DESTINATION bin)
  install (FILES ${3RDPARTY_OCCT_LIBRARY_DIR}/libTKGeomBase.so  DESTINATION bin)
  install (FILES ${3RDPARTY_OCCT_LIBRARY_DIR}/libTKIGES.so      DESTINATION bin)
  install (FILES ${3RDPARTY_OCCT_LIBRARY_DIR}/libTKLCAF.so      DESTINATION bin)
  install (FILES ${3RDPARTY_OCCT_LIBRARY_DIR}/libTKMath.so      DESTINATION bin)
  install (FILES ${3RDPARTY_OCCT_LIBRARY_DIR}/libTKMesh.so      DESTINATION bin)
  install (FILES ${3RDPARTY_OCCT_LIBRARY_DIR}/libTKOffset.so    DESTINATION bin)
  install (FILES ${3RDPARTY_OCCT_LIBRARY_DIR}/libTKPrim.so      DESTINATION bin)
  install (FILES ${3RDPARTY_OCCT_LIBRARY_DIR}/libTKShHealing.so DESTINATION bin)
  install (FILES ${3RDPARTY_OCCT_LIBRARY_DIR}/libTKSTEP.so      DESTINATION bin)
  install (FILES ${3RDPARTY_OCCT_LIBRARY_DIR}/libTKSTEP209.so   DESTINATION bin)
  install (FILES ${3RDPARTY_OCCT_LIBRARY_DIR}/libTKSTEPAttr.so  DESTINATION bin)
  install (FILES ${3RDPARTY_OCCT_LIBRARY_DIR}/libTKSTEPBase.so  DESTINATION bin)
  install (FILES ${3RDPARTY_OCCT_LIBRARY_DIR}/libTKTopAlgo.so   DESTINATION bin)
  install (FILES ${3RDPARTY_OCCT_LIBRARY_DIR}/libTKXSBase.so    DESTINATION bin)
  install (FILES ${3RDPARTY_OCCT_LIBRARY_DIR}/libTKHLR.so       DESTINATION bin)
  install (FILES ${3RDPARTY_OCCT_LIBRARY_DIR}/libTKFillet.so    DESTINATION bin)
  install (FILES ${3RDPARTY_OCCT_LIBRARY_DIR}/libTKSTL.so       DESTINATION bin)
  install (FILES ${3RDPARTY_OCCT_LIBRARY_DIR}/libTKXCAF.so      DESTINATION bin)
  install (FILES ${3RDPARTY_OCCT_LIBRARY_DIR}/libTKVCAF.so      DESTINATION bin)
  install (FILES ${3RDPARTY_OCCT_LIBRARY_DIR}/libTKXDESTEP.so   DESTINATION bin)
  install (FILES ${3RDPARTY_OCCT_LIBRARY_DIR}/libTKV3d.so       DESTINATION bin)
  install (FILES ${3RDPARTY_OCCT_LIBRARY_DIR}/libTKService.so   DESTINATION bin)
  #
  install (FILES ${3RDPARTY_OCCT_LIBRARY_DIR}/libTKBin.so.7       DESTINATION bin)
  install (FILES ${3RDPARTY_OCCT_LIBRARY_DIR}/libTKBinL.so.7      DESTINATION bin)
  install (FILES ${3RDPARTY_OCCT_LIBRARY_DIR}/libTKBO.so.7        DESTINATION bin)
  install (FILES ${3RDPARTY_OCCT_LIBRARY_DIR}/libTKBool.so.7      DESTINATION bin)
  install (FILES ${3RDPARTY_OCCT_LIBRARY_DIR}/libTKBRep.so.7      DESTINATION bin)
  install (FILES ${3RDPARTY_OCCT_LIBRARY_DIR}/libTKCAF.so.7       DESTINATION bin)
  install (FILES ${3RDPARTY_OCCT_LIBRARY_DIR}/libTKCDF.so.7       DESTINATION bin)
  install (FILES ${3RDPARTY_OCCT_LIBRARY_DIR}/libTKernel.so.7     DESTINATION bin)
  install (FILES ${3RDPARTY_OCCT_LIBRARY_DIR}/libTKG2d.so.7       DESTINATION bin)
  install (FILES ${3RDPARTY_OCCT_LIBRARY_DIR}/libTKG3d.so.7       DESTINATION bin)
  install (FILES ${3RDPARTY_OCCT_LIBRARY_DIR}/libTKGeomAlgo.so.7  DESTINATION bin)
  install (FILES ${3RDPARTY_OCCT_LIBRARY_DIR}/libTKGeomBase.so.7  DESTINATION bin)
  install (FILES ${3RDPARTY_OCCT_LIBRARY_DIR}/libTKIGES.so.7      DESTINATION bin)
  install (FILES ${3RDPARTY_OCCT_LIBRARY_DIR}/libTKLCAF.so.7      DESTINATION bin)
  install (FILES ${3RDPARTY_OCCT_LIBRARY_DIR}/libTKMath.so.7      DESTINATION bin)
  install (FILES ${3RDPARTY_OCCT_LIBRARY_DIR}/libTKMesh.so.7      DESTINATION bin)
  install (FILES ${3RDPARTY_OCCT_LIBRARY_DIR}/libTKOffset.so.7    DESTINATION bin)
  install (FILES ${3RDPARTY_OCCT_LIBRARY_DIR}/libTKPrim.so.7      DESTINATION bin)
  install (FILES ${3RDPARTY_OCCT_LIBRARY_DIR}/libTKShHealing.so.7 DESTINATION bin)
  install (FILES ${3RDPARTY_OCCT_LIBRARY_DIR}/libTKSTEP.so.7      DESTINATION bin)
  install (FILES ${3RDPARTY_OCCT_LIBRARY_DIR}/libTKSTEP209.so.7   DESTINATION bin)
  install (FILES ${3RDPARTY_OCCT_LIBRARY_DIR}/libTKSTEPAttr.so.7  DESTINATION bin)
  install (FILES ${3RDPARTY_OCCT_LIBRARY_DIR}/libTKSTEPBase.so.7  DESTINATION bin)
  install (FILES ${3RDPARTY_OCCT_LIBRARY_DIR}/libTKTopAlgo.so.7   DESTINATION bin)
  install (FILES ${3RDPARTY_OCCT_LIBRARY_DIR}/libTKXSBase.so.7    DESTINATION bin)
  install (FILES ${3RDPARTY_OCCT_LIBRARY_DIR}/libTKHLR.so.7       DESTINATION bin)
  install (FILES ${3RDPARTY_OCCT_LIBRARY_DIR}/libTKFillet.so.7    DESTINATION bin)
  install (FILES ${3RDPARTY_OCCT_LIBRARY_DIR}/libTKSTL.so.7       DESTINATION bin)
  install (FILES ${3RDPARTY_OCCT_LIBRARY_DIR}/libTKXCAF.so.7      DESTINATION bin)
  install (FILES ${3RDPARTY_OCCT_LIBRARY_DIR}/libTKVCAF.so.7      DESTINATION bin)
  install (FILES ${3RDPARTY_OCCT_LIBRARY_DIR}/libTKXDESTEP.so.7   DESTINATION bin)
  install (FILES ${3RDPARTY_OCCT_LIBRARY_DIR}/libTKV3d.so.7       DESTINATION bin)
  install (FILES ${3RDPARTY_OCCT_LIBRARY_DIR}/libTKService.so.7   DESTINATION bin)
  #
  install (FILES ${3RDPARTY_OCCT_LIBRARY_DIR}/libTKBin.so.7.4.0       DESTINATION bin)
  install (FILES ${3RDPARTY_OCCT_LIBRARY_DIR}/libTKBinL.so.7.4.0      DESTINATION bin)
  install (FILES ${3RDPARTY_OCCT_LIBRARY_DIR}/libTKBO.so.7.4.0        DESTINATION bin)
  install (FILES ${3RDPARTY_OCCT_LIBRARY_DIR}/libTKBool.so.7.4.0      DESTINATION bin)
  install (FILES ${3RDPARTY_OCCT_LIBRARY_DIR}/libTKBRep.so.7.4.0      DESTINATION bin)
  install (FILES ${3RDPARTY_OCCT_LIBRARY_DIR}/libTKCAF.so.7.4.0       DESTINATION bin)
  install (FILES ${3RDPARTY_OCCT_LIBRARY_DIR}/libTKCDF.so.7.4.0       DESTINATION bin)
  install (FILES ${3RDPARTY_OCCT_LIBRARY_DIR}/libTKernel.so.7.4.0     DESTINATION bin)
  install (FILES ${3RDPARTY_OCCT_LIBRARY_DIR}/libTKG2d.so.7.4.0       DESTINATION bin)
  install (FILES ${3RDPARTY_OCCT_LIBRARY_DIR}/libTKG3d.so.7.4.0       DESTINATION bin)
  install (FILES ${3RDPARTY_OCCT_LIBRARY_DIR}/libTKGeomAlgo.so.7.4.0  DESTINATION bin)
  install (FILES ${3RDPARTY_OCCT_LIBRARY_DIR}/libTKGeomBase.so.7.4.0  DESTINATION bin)
  install (FILES ${3RDPARTY_OCCT_LIBRARY_DIR}/libTKIGES.so.7.4.0      DESTINATION bin)
  install (FILES ${3RDPARTY_OCCT_LIBRARY_DIR}/libTKLCAF.so.7.4.0      DESTINATION bin)
  install (FILES ${3RDPARTY_OCCT_LIBRARY_DIR}/libTKMath.so.7.4.0      DESTINATION bin)
  install (FILES ${3RDPARTY_OCCT_LIBRARY_DIR}/libTKMesh.so.7.4.0      DESTINATION bin)
  install (FILES ${3RDPARTY_OCCT_LIBRARY_DIR}/libTKOffset.so.7.4.0    DESTINATION bin)
  install (FILES ${3RDPARTY_OCCT_LIBRARY_DIR}/libTKPrim.so.7.4.0      DESTINATION bin)
  install (FILES ${3RDPARTY_OCCT_LIBRARY_DIR}/libTKShHealing.so.7.4.0 DESTINATION bin)
  install (FILES ${3RDPARTY_OCCT_LIBRARY_DIR}/libTKSTEP.so.7.4.0      DESTINATION bin)
  install (FILES ${3RDPARTY_OCCT_LIBRARY_DIR}/libTKSTEP209.so.7.4.0   DESTINATION bin)
  install (FILES ${3RDPARTY_OCCT_LIBRARY_DIR}/libTKSTEPAttr.so.7.4.0  DESTINATION bin)
  install (FILES ${3RDPARTY_OCCT_LIBRARY_DIR}/libTKSTEPBase.so.7.4.0  DESTINATION bin)
  install (FILES ${3RDPARTY_OCCT_LIBRARY_DIR}/libTKTopAlgo.so.7.4.0   DESTINATION bin)
  install (FILES ${3RDPARTY_OCCT_LIBRARY_DIR}/libTKXSBase.so.7.4.0    DESTINATION bin)
  install (FILES ${3RDPARTY_OCCT_LIBRARY_DIR}/libTKHLR.so.7.4.0       DESTINATION bin)
  install (FILES ${3RDPARTY_OCCT_LIBRARY_DIR}/libTKFillet.so.7.4.0    DESTINATION bin)
  install (FILES ${3RDPARTY_OCCT_LIBRARY_DIR}/libTKSTL.so.7.4.0       DESTINATION bin)
  install (FILES ${3RDPARTY_OCCT_LIBRARY_DIR}/libTKXCAF.so.7.4.0      DESTINATION bin)
  install (FILES ${3RDPARTY_OCCT_LIBRARY_DIR}/libTKVCAF.so.7.4.0      DESTINATION bin)
  install (FILES ${3RDPARTY_OCCT_LIBRARY_DIR}/libTKXDESTEP.so.7.4.0   DESTINATION bin)
  install (FILES ${3RDPARTY_OCCT_LIBRARY_DIR}/libTKV3d.so.7.4.0       DESTINATION bin)
  install (FILES ${3RDPARTY_OCCT_LIBRARY_DIR}/libTKService.so.7.4.0   DESTINATION bin)
endif()
