CFLAGS += -DCONFIG_ACPICA

OBJS += $(ACPICA_DIR)/oscell.o

OBJS += $(ACPICA_DIR)/events/evevent.o	\
	$(ACPICA_DIR)/events/evgpe.o	\
	$(ACPICA_DIR)/events/evgpeblk.o	\
	$(ACPICA_DIR)/events/evmisc.o	\
	$(ACPICA_DIR)/events/evregion.o	\
	$(ACPICA_DIR)/events/evrgnini.o	\
	$(ACPICA_DIR)/events/evsci.o	\
	$(ACPICA_DIR)/events/evxface.o	\
	$(ACPICA_DIR)/events/evxfevnt.o	\
	$(ACPICA_DIR)/events/evxfregn.o	\
	$(ACPICA_DIR)/events/evgpeutil.o \
	$(ACPICA_DIR)/events/evgpeinit.o

OBJS += $(ACPICA_DIR)/hardware/hwacpi.o \
	$(ACPICA_DIR)/hardware/hwgpe.o 	 \
	$(ACPICA_DIR)/hardware/hwregs.o  \
	$(ACPICA_DIR)/hardware/hwsleep.o \
	$(ACPICA_DIR)/hardware/hwtimer.o \
	$(ACPICA_DIR)/hardware/hwvalid.o \
	$(ACPICA_DIR)/hardware/hwxface.o \
	$(ACPICA_DIR)/hardware/hwpci.o 


OBJS += $(ACPICA_DIR)/debugger/dbdisply.o \
	$(ACPICA_DIR)/debugger/dbxface.o \
	

OBJS + = $(ACPICA_DIR)/disassembler/dmobject.o \
		 $(ACPICA_DIR)/disassembler/dmresrc.o \
		 $(ACPICA_DIR)/disassembler/dmopcode.o

# Note that the "compiler" directory files are not included


OBJS += $(ACPICA_DIR)/dispatcher/dsfield.o \
	$(ACPICA_DIR)/dispatcher/dsinit.o \
	$(ACPICA_DIR)/dispatcher/dsmethod.o \
	$(ACPICA_DIR)/dispatcher/dsmthdat.o \
	$(ACPICA_DIR)/dispatcher/dsobject.o \
	$(ACPICA_DIR)/dispatcher/dsopcode.o \
	$(ACPICA_DIR)/dispatcher/dsutils.o \
	$(ACPICA_DIR)/dispatcher/dswexec.o \
	$(ACPICA_DIR)/dispatcher/dswload.o \
	$(ACPICA_DIR)/dispatcher/dswscope.o \
	$(ACPICA_DIR)/dispatcher/dswstate.o 


OBJS += $(ACPICA_DIR)/executer/exconfig.o \
	$(ACPICA_DIR)/executer/exconvrt.o \
	$(ACPICA_DIR)/executer/excreate.o \
	$(ACPICA_DIR)/executer/exdump.o \
	$(ACPICA_DIR)/executer/exfield.o \
	$(ACPICA_DIR)/executer/exfldio.o \
	$(ACPICA_DIR)/executer/exmisc.o \
	$(ACPICA_DIR)/executer/exmutex.o \
	$(ACPICA_DIR)/executer/exnames.o \
	$(ACPICA_DIR)/executer/exoparg1.o \
	$(ACPICA_DIR)/executer/exoparg2.o \
	$(ACPICA_DIR)/executer/exoparg3.o \
	$(ACPICA_DIR)/executer/exoparg6.o \
	$(ACPICA_DIR)/executer/exprep.o \
	$(ACPICA_DIR)/executer/exregion.o \
	$(ACPICA_DIR)/executer/exresnte.o \
	$(ACPICA_DIR)/executer/exresolv.o \
	$(ACPICA_DIR)/executer/exresop.o \
	$(ACPICA_DIR)/executer/exstore.o \
	$(ACPICA_DIR)/executer/exstoren.o \
	$(ACPICA_DIR)/executer/exstorob.o \
	$(ACPICA_DIR)/executer/exsystem.o \
	$(ACPICA_DIR)/executer/exutils.o \
	$(ACPICA_DIR)/executer/exdebug.o


OBJS += $(ACPICA_DIR)/parser/psargs.o \
	$(ACPICA_DIR)/parser/psloop.o \
	$(ACPICA_DIR)/parser/psopcode.o \
	$(ACPICA_DIR)/parser/psparse.o \
	$(ACPICA_DIR)/parser/psscope.o \
	$(ACPICA_DIR)/parser/pstree.o \
	$(ACPICA_DIR)/parser/psutils.o \
	$(ACPICA_DIR)/parser/pswalk.o \
	$(ACPICA_DIR)/parser/psxface.o 


OBJS += $(ACPICA_DIR)/namespace/nsaccess.o \
	$(ACPICA_DIR)/namespace/nsalloc.o \
	$(ACPICA_DIR)/namespace/nsdump.o \
	$(ACPICA_DIR)/namespace/nsdumpdv.o \
	$(ACPICA_DIR)/namespace/nseval.o \
	$(ACPICA_DIR)/namespace/nsinit.o \
	$(ACPICA_DIR)/namespace/nsload.o \
	$(ACPICA_DIR)/namespace/nsnames.o \
	$(ACPICA_DIR)/namespace/nsobject.o \
	$(ACPICA_DIR)/namespace/nsparse.o \
	$(ACPICA_DIR)/namespace/nspredef.o \
	$(ACPICA_DIR)/namespace/nsrepair.o \
	$(ACPICA_DIR)/namespace/nssearch.o \
	$(ACPICA_DIR)/namespace/nsutils.o \
	$(ACPICA_DIR)/namespace/nswalk.o \
	$(ACPICA_DIR)/namespace/nsxfeval.o \
	$(ACPICA_DIR)/namespace/nsxfname.o \
	$(ACPICA_DIR)/namespace/nsxfobj.o \
	$(ACPICA_DIR)/namespace/nsrepair2.o


OBJS += $(ACPICA_DIR)/resources/rsaddr.o \
	$(ACPICA_DIR)/resources/rscalc.o \
	$(ACPICA_DIR)/resources/rscreate.o \
	$(ACPICA_DIR)/resources/rsdump.o \
	$(ACPICA_DIR)/resources/rsinfo.o \
	$(ACPICA_DIR)/resources/rsio.o \
	$(ACPICA_DIR)/resources/rsirq.o \
	$(ACPICA_DIR)/resources/rslist.o \
	$(ACPICA_DIR)/resources/rsmemory.o \
	$(ACPICA_DIR)/resources/rsmisc.o \
	$(ACPICA_DIR)/resources/rsutils.o \
	$(ACPICA_DIR)/resources/rsxface.o


OBJS += $(ACPICA_DIR)/tables/tbfadt.o \
	$(ACPICA_DIR)/tables/tbfind.o \
	$(ACPICA_DIR)/tables/tbinstal.o \
	$(ACPICA_DIR)/tables/tbutils.o \
	$(ACPICA_DIR)/tables/tbxface.o \
	$(ACPICA_DIR)/tables/tbxfroot.o


OBJS += $(ACPICA_DIR)/utilities/utalloc.o \
	$(ACPICA_DIR)/utilities/utcache.o \
	$(ACPICA_DIR)/utilities/utclib.o \
	$(ACPICA_DIR)/utilities/utcopy.o \
	$(ACPICA_DIR)/utilities/utdebug.o \
	$(ACPICA_DIR)/utilities/utdelete.o \
	$(ACPICA_DIR)/utilities/uteval.o \
	$(ACPICA_DIR)/utilities/utglobal.o \
	$(ACPICA_DIR)/utilities/utids.o \
	$(ACPICA_DIR)/utilities/utinit.o \
	$(ACPICA_DIR)/utilities/utlock.o \
	$(ACPICA_DIR)/utilities/utmath.o \
	$(ACPICA_DIR)/utilities/utmisc.o \
	$(ACPICA_DIR)/utilities/utmutex.o \
	$(ACPICA_DIR)/utilities/utobject.o \
	$(ACPICA_DIR)/utilities/utresrc.o \
	$(ACPICA_DIR)/utilities/utstate.o \
	$(ACPICA_DIR)/utilities/uttrack.o \
	$(ACPICA_DIR)/utilities/utxface.o \
	$(ACPICA_DIR)/utilities/utxferror.o \
	$(ACPICA_DIR)/utilities/utosi.o


