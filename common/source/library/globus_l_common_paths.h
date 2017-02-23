struct globus_l_common_path_lookup_table_s
{
    const char * name;
    const char * path;
};

static struct globus_l_common_path_lookup_table_s
globus_l_common_path_lookup_table[] =
{
    { "prefix", "/home/esjung/gt6_inst" },
    { "exec_prefix", "${prefix}" },
    { "sbindir", "${exec_prefix}/sbin" },
    { "bindir", "${exec_prefix}/bin" },
    { "libdir", "${exec_prefix}/lib" },
    { "libexecdir", "${exec_prefix}/libexec" },
    { "includedir", "${prefix}/include" },
    { "datarootdir", "${prefix}/share" },
    { "datadir", "${datarootdir}" },
    { "mandir", "${datarootdir}/man" },
    { "sysconfdir", "${prefix}/etc" },
    { "sharedstatedir", "${prefix}/com" },
    { "localstatedir", "${prefix}/var" },
    { "perlmoduledir", "${libdir}/perl" },
    { NULL, NULL }
};
