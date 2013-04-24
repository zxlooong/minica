
Evpps.dll: dlldata.obj Evp_p.obj Evp_i.obj
	link /dll /out:Evpps.dll /def:Evpps.def /entry:DllMain dlldata.obj Evp_p.obj Evp_i.obj \
		kernel32.lib rpcndr.lib rpcns4.lib rpcrt4.lib oleaut32.lib uuid.lib \

.c.obj:
	cl /c /Ox /DWIN32 /D_WIN32_WINNT=0x0400 /DREGISTER_PROXY_DLL \
		$<

clean:
	@del Evpps.dll
	@del Evpps.lib
	@del Evpps.exp
	@del dlldata.obj
	@del Evp_p.obj
	@del Evp_i.obj
