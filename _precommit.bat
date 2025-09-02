@rem Set up current dir as OS current dir, and then launch this file .\_precommit.bat
@rem
@rem --- Fix tabs and traling spaces
@rem 
@set EMPTY=
@rem 
@set PROCESS_FILES_MASKS=*.cpp^,*.c^,*.cc^,*.cxx^,*.c++^,*.hpp^,*.h^,*.hxx^,*.h++^,*.md_^ %EMPTY%
@set OPT_PROCESS_FILES=--include-files=%PROCESS_FILES_MASKS%
@rem 
@set EXCLUDE_FILES_MASKS=*/_libs/*^ %EMPTY%
@set OPT_EXCLUDE_FILES=--exclude-files=%EXCLUDE_FILES_MASKS%
@rem 
@set TABTOOL_TAB_OPTS=--tab-size=4 --tab-delta=1 --tab --remove-trailing-spaces=Y
@rem 
@rem Uncoment next line to call fix tool
@rem umba-tabtool %TABTOOL_TAB_OPTS% %OPT_PROCESS_FILES% %OPT_EXCLUDE_FILES% --scan "%~dp0\../doc" "%~dp0\../src" "%~dp0\../tests"
@rem 
@rem 
@rem --- Sources statistic
@rem By cloc - https://github.com/AlDanial/cloc
@set CLOCOUTFORMATOPT=
@set CLOCOUTFORMATEXT=txt
@call :RUN_CLOCK
@set CLOCOUTFORMATOPT=--md
@set CLOCOUTFORMATEXT=md_
@call :RUN_CLOCK
@goto MODULE_PRECOMMIT_CONTINUE01

:RUN_CLOCK
cloc --vcs=git %CLOCOUTFORMATOPT%                  --by-file-by-lang --out=.\md_\src-stat\stat.%CLOCOUTFORMATEXT%             .
cloc --vcs=git %CLOCOUTFORMATOPT% --by-percent cmb --by-file-by-lang --out=.\md_\src-stat\stat-by-percent.%CLOCOUTFORMATEXT%  .
exit /B 0

:MODULE_PRECOMMIT_CONTINUE01

@rem 
@rem call "%~dp0\.bat\gen-doc.bat"

