DFHACK_PLUGIN("twbt");

DFhackCExport command_result plugin_init ( color_ostream &out, vector <PluginCommand> &commands)
{
    out2 = &out;

    *out2 << "TWBT: version " << TWBT_VER << std::endl;
#ifdef NO_RENDERING_PATCH
    *out2 << COLOR_YELLOW << "TWBT: no rendering patch" << std::endl;
    *out2 << COLOR_RESET;
#endif
#ifdef NO_DISPLAY_PATCH
    *out2 << COLOR_YELLOW << "TWBT: no display patch" << std::endl;
    *out2 << COLOR_RESET;
#endif

    auto dflags = init->display.flag;
    if (dflags.is_set(init_display_flags::RENDER_2D) ||
        dflags.is_set(init_display_flags::ACCUM_BUFFER) ||
        dflags.is_set(init_display_flags::FRAME_BUFFER) ||
        dflags.is_set(init_display_flags::TEXT) ||
        dflags.is_set(init_display_flags::VBO) ||
        dflags.is_set(init_display_flags::PARTIAL_PRINT))
    {
        *out2 << COLOR_RED << "TWBT: PRINT_MODE must be set to STANDARD in init.txt" << std::endl;
        *out2 << COLOR_RESET;
        return CR_OK;        
    }

    #ifdef WIN32
        _load_multi_pdim = (LOAD_MULTI_PDIM) (A_LOAD_MULTI_PDIM + Core::getInstance().vinfo->getRebaseDelta());
        _render_map = (RENDER_MAP) (A_RENDER_MAP + Core::getInstance().vinfo->getRebaseDelta());
        _render_updown = (RENDER_UPDOWN) (A_RENDER_MAP + Core::getInstance().vinfo->getRebaseDelta());
    #elif defined(__APPLE__)
        _load_multi_pdim = (LOAD_MULTI_PDIM) A_LOAD_MULTI_PDIM;    
        _render_map = (RENDER_MAP) A_RENDER_MAP;
        _render_updown = (RENDER_UPDOWN) A_RENDER_MAP;
    #else
        _load_multi_pdim = (LOAD_MULTI_PDIM) dlsym(RTLD_DEFAULT, "_ZN8textures15load_multi_pdimERKSsPlllbS2_S2_");
        _render_map = (RENDER_MAP) A_RENDER_MAP;
        _render_updown = (RENDER_UPDOWN) A_RENDER_MAP;
    #endif

    bad_item_flags.whole = 0;
    bad_item_flags.bits.in_building = true;
    bad_item_flags.bits.garbage_collect = true;
    bad_item_flags.bits.removed = true;
    bad_item_flags.bits.dead_dwarf = true;
    bad_item_flags.bits.murder = true;
    bad_item_flags.bits.construction = true;
    bad_item_flags.bits.in_inventory = true;
    bad_item_flags.bits.in_chest = true;

    // Used only if rendering patch is not available
    skytile = d_init->sky_tile;
    chasmtile = d_init->chasm_tile;    

    // Graphics tileset - accessible at index 0
    struct tileset ts;
    memcpy(ts.small_texpos, init->font.small_font_texpos, sizeof(ts.small_texpos));
    memcpy(ts.large_texpos, init->font.large_font_texpos, sizeof(ts.large_texpos));
    tilesets.push_back(ts);

    // We will replace init->font with text font, so let's save graphics tile size
    small_map_dispx = init->font.small_font_dispx, small_map_dispy = init->font.small_font_dispy;
    large_map_dispx = init->font.large_font_dispx, large_map_dispy = init->font.large_font_dispy;

    has_textfont = load_text_font();
    has_overrides = load_overrides();

    if (!has_textfont)
    {
        *out2 << COLOR_YELLOW << "TWBT: FONT and GRAPHICS_FONT are the same" << std::endl;
        *out2 << COLOR_RESET;
    }

    // Load shadows
    struct stat buf;
    if (stat("data/art/shadows.png", &buf) == 0)
    {
        long dx, dy;        
        load_tileset("data/art/shadows.png", shadow_texpos, 8, 1, &dx, &dy);
        shadowsloaded = true;
    }
    else
    {
        *out2 << COLOR_RED << "TWBT: shadows.png not found in data/art folder" << std::endl;
        *out2 << COLOR_RESET;
    }

    map_texpos = enabler->fullscreen ? tilesets[0].large_texpos : tilesets[0].small_texpos;
    text_texpos = enabler->fullscreen ? tilesets[1].large_texpos : tilesets[1].small_texpos;

    replace_renderer();

    INTERPOSE_HOOK(dwarfmode_hook, render).apply(true);
    INTERPOSE_HOOK(dwarfmode_hook, feed).apply(true);

    INTERPOSE_HOOK(dungeonmode_hook, render).apply(true);
    INTERPOSE_HOOK(dungeonmode_hook, logic).apply(true);
    INTERPOSE_HOOK(dungeonmode_hook, feed).apply(true);

#if defined(__APPLE__) && defined(DF_03411)
    INTERPOSE_HOOK(traderesize_hook, render).apply(true);
#endif    

    commands.push_back(PluginCommand(
        "mapshot", "Mapshot!",
        mapshot_cmd, true,
        ""
    ));        
    commands.push_back(PluginCommand(
        "multilevel", "Multi-level rendering",
        multilevel_cmd, false,
        ""
    ));       
    commands.push_back(PluginCommand(
        "colormap", "Colomap manipulation",
        colormap_cmd, false,
        ""
    ));
    commands.push_back(PluginCommand(
        "twbt", "Text Will Be Text",
        twbt_cmd, false,
        ""
    ));   

    return CR_OK;
}

DFhackCExport command_result plugin_onstatechange(color_ostream &out, state_change_event event)
{
    if (event == SC_WORLD_LOADED)
    {
        update_custom_building_overrides();
        gmenu_w = -1;
    }

    return CR_OK;
}

DFhackCExport command_result plugin_shutdown ( color_ostream &out )
{
    return CR_FAILURE;

    /*if (enabled)
        restore_renderer();

#if defined(__APPLE__) && defined(DF_03411)
    INTERPOSE_HOOK(traderesize_hook, render).apply(false);
#endif            

    return CR_OK;*/
}