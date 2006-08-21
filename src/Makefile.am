SUBDIRS = sting

AM_CFLAGS = -Wall

bin_PROGRAMS = hexen

header_files = am_data.h am_map.h ct_chat.h dstrings.h h2def.h \
	i_audio.h i_music.h i_net.h info.h i_qmus2mid.h i_sound.h i_system.h \
	i_video.h p_local.h p_spec.h r_local.h sounds.h soundst.h st_start.h \
	textdefs.h xddefs.h m_swap.h m_fixed.h i_cdmus.h

hexen_SOURCES = a_action.c am_map.c ct_chat.c d_net.c f_finale.c g_game.c \
	h2_main.c i_audio.c i_music.c i_net.c i_net_sting.c i_net_unix.c info.c \
	in_lude.c i_qmus2mid.c i_sound.c i_system.c i_video.c m_misc.c mn_menu.c \
	p_acs.c p_anim.c p_ceilng.c p_doors.c p_enemy.c p_floor.c p_inter.c \
	p_lights.c p_map.c p_maputl.c p_mobj.c po_man.c p_plats.c p_pspr.c \
	p_setup.c p_sight.c p_spec.c p_switch.c p_telept.c p_things.c p_tick.c \
	p_user.c r_bsp.c r_data.c r_draw.c r_main.c r_plane.c r_segs.c r_things.c \
	sb_bar.c sc_man.c sn_sonix.c sounds.c st_start.c sv_save.c tables.c \
	v_video.c w_wad.c z_zone.c m_swap.c m_fixed.c soundst.c i_cdmus.c

EXTRA_DIST = $(header_files)
