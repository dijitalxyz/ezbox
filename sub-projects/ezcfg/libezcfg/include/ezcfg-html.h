/* ============================================================================
 * Project Name : ezbox configuration utilities
 * File Name    : ezcfg-html.h
 *
 * Description  : interface to configurate ezbox information
 *
 * Copyright (C) 2008-2013 by ezbox-project
 *
 * History      Rev       Description
 * 2011-03-26   0.1       Write it from scratch
 * ============================================================================
 */

#ifndef _EZCFG_HTML_H_
#define _EZCFG_HTML_H_

/* ezcfg HTML4.01 element names */
#define EZCFG_HTML_A_ELEMENT_NAME           "a"
#define EZCFG_HTML_ABBR_ELEMENT_NAME        "abbr"
#define EZCFG_HTML_ACRONYM_ELEMENT_NAME     "acronym"
#define EZCFG_HTML_ADDRESS_ELEMENT_NAME     "address"
//#define EZCFG_HTML_APPLET_ELEMENT_NAME      "applet"
#define EZCFG_HTML_AREA_ELEMENT_NAME        "area"
#define EZCFG_HTML_B_ELEMENT_NAME           "b"
#define EZCFG_HTML_BASE_ELEMENT_NAME        "base"
//#define EZCFG_HTML_BASEFONT_ELEMENT_NAME    "basefont"
#define EZCFG_HTML_BDO_ELEMENT_NAME         "bdo"
#define EZCFG_HTML_BIG_ELEMENT_NAME         "big"
#define EZCFG_HTML_BLOCKQUOTE_ELEMENT_NAME  "blockquote"
#define EZCFG_HTML_BODY_ELEMENT_NAME        "body"
#define EZCFG_HTML_BR_ELEMENT_NAME          "br"
#define EZCFG_HTML_BUTTON_ELEMENT_NAME      "button"
#define EZCFG_HTML_CAPTION_ELEMENT_NAME     "caption"
//#define EZCFG_HTML_CENTER_ELEMENT_NAME      "center"
#define EZCFG_HTML_CITE_ELEMENT_NAME        "cite"
#define EZCFG_HTML_CODE_ELEMENT_NAME        "code"
#define EZCFG_HTML_COL_ELEMENT_NAME         "col"
#define EZCFG_HTML_COLGROUP_ELEMENT_NAME    "colgroup"
#define EZCFG_HTML_DD_ELEMENT_NAME          "dd"
#define EZCFG_HTML_DEL_ELEMENT_NAME         "del"
#define EZCFG_HTML_DFN_ELEMENT_NAME         "dfn"
//#define EZCFG_HTML_DIR_ELEMENT_NAME         "dir"
#define EZCFG_HTML_DIV_ELEMENT_NAME         "div"
#define EZCFG_HTML_DL_ELEMENT_NAME          "dl"
#define EZCFG_HTML_DT_ELEMENT_NAME          "dt"
#define EZCFG_HTML_EM_ELEMENT_NAME          "em"
#define EZCFG_HTML_FIELDSET_ELEMENT_NAME    "fieldset"
//#define EZCFG_HTML_FONT_ELEMENT_NAME        "font"
#define EZCFG_HTML_FORM_ELEMENT_NAME        "form"
//#define EZCFG_HTML_FRAME_ELEMENT_NAME       "frame"
//#define EZCFG_HTML_FRAMESET_ELEMENT_NAME    "frameset"
#define EZCFG_HTML_H1_ELEMENT_NAME          "h1"
#define EZCFG_HTML_H2_ELEMENT_NAME          "h2"
#define EZCFG_HTML_H3_ELEMENT_NAME          "h3"
#define EZCFG_HTML_H4_ELEMENT_NAME          "h4"
#define EZCFG_HTML_H5_ELEMENT_NAME          "h5"
#define EZCFG_HTML_H6_ELEMENT_NAME          "h6"
#define EZCFG_HTML_HEAD_ELEMENT_NAME        "head"
#define EZCFG_HTML_HR_ELEMENT_NAME          "hr"
#define EZCFG_HTML_HTML_ELEMENT_NAME        "html"
#define EZCFG_HTML_I_ELEMENT_NAME           "i"
//#define EZCFG_HTML_IFRAME_ELEMENT_NAME      "iframe"
#define EZCFG_HTML_IMG_ELEMENT_NAME         "img"
#define EZCFG_HTML_INPUT_ELEMENT_NAME       "input"
#define EZCFG_HTML_INS_ELEMENT_NAME         "ins"
//#define EZCFG_HTML_ISINDEX_ELEMENT_NAME     "isindex"
#define EZCFG_HTML_KBD_ELEMENT_NAME         "kbd"
#define EZCFG_HTML_LABEL_ELEMENT_NAME       "label"
#define EZCFG_HTML_LEGEND_ELEMENT_NAME      "legend"
#define EZCFG_HTML_LI_ELEMENT_NAME          "li"
#define EZCFG_HTML_LINK_ELEMENT_NAME        "link"
#define EZCFG_HTML_MAP_ELEMENT_NAME         "map"
//#define EZCFG_HTML_MENU_ELEMENT_NAME        "menu"
#define EZCFG_HTML_META_ELEMENT_NAME        "meta"
//#define EZCFG_HTML_NOFRAMES_ELEMENT_NAME    "noframes"
#define EZCFG_HTML_NOSCRIPT_ELEMENT_NAME    "noscript"
#define EZCFG_HTML_OBJECT_ELEMENT_NAME      "object"
#define EZCFG_HTML_OL_ELEMENT_NAME          "ol"
#define EZCFG_HTML_OPTGROUP_ELEMENT_NAME    "optgroup"
#define EZCFG_HTML_OPTION_ELEMENT_NAME      "option"
#define EZCFG_HTML_P_ELEMENT_NAME           "p"
#define EZCFG_HTML_PARAM_ELEMENT_NAME       "param"
#define EZCFG_HTML_PRE_ELEMENT_NAME         "pre"
#define EZCFG_HTML_Q_ELEMENT_NAME           "q"
//#define EZCFG_HTML_S_ELEMENT_NAME           "s"
#define EZCFG_HTML_SAMP_ELEMENT_NAME        "samp"
#define EZCFG_HTML_SCRIPT_ELEMENT_NAME      "script"
#define EZCFG_HTML_SELECT_ELEMENT_NAME      "select"
#define EZCFG_HTML_SMALL_ELEMENT_NAME       "small"
#define EZCFG_HTML_SPAN_ELEMENT_NAME        "span"
//#define EZCFG_HTML_STRIKE_ELEMENT_NAME      "strike"
#define EZCFG_HTML_STRONG_ELEMENT_NAME      "strong"
#define EZCFG_HTML_STYLE_ELEMENT_NAME       "style"
#define EZCFG_HTML_SUB_ELEMENT_NAME         "sub"
#define EZCFG_HTML_SUP_ELEMENT_NAME         "sup"
#define EZCFG_HTML_TABLE_ELEMENT_NAME       "table"
#define EZCFG_HTML_TBODY_ELEMENT_NAME       "tbody"
#define EZCFG_HTML_TD_ELEMENT_NAME          "td"
#define EZCFG_HTML_TEXTAREA_ELEMENT_NAME    "textarea"
#define EZCFG_HTML_TFOOT_ELEMENT_NAME       "tfoot"
#define EZCFG_HTML_TH_ELEMENT_NAME          "th"
#define EZCFG_HTML_THEAD_ELEMENT_NAME       "thead"
#define EZCFG_HTML_TITLE_ELEMENT_NAME       "title"
#define EZCFG_HTML_TR_ELEMENT_NAME          "tr"
#define EZCFG_HTML_TT_ELEMENT_NAME          "tt"
//#define EZCFG_HTML_U_ELEMENT_NAME           "u"
#define EZCFG_HTML_UL_ELEMENT_NAME          "ul"
#define EZCFG_HTML_VAR_ELEMENT_NAME         "var"

/* ezcfg HTML4.01 attribute names */
#define EZCFG_HTML_ABBR_ATTRIBUTE_NAME              "abbr"
#define EZCFG_HTML_ACCEPT_CHARSET_ATTRIBUTE_NAME    "accept-charset"
#define EZCFG_HTML_ACCEPT_ATTRIBUTE_NAME            "accept"
#define EZCFG_HTML_ACCEPTKEY_ATTRIBUTE_NAME         "accesskey"
#define EZCFG_HTML_ACTION_ATTRIBUTE_NAME            "action"
#define EZCFG_HTML_ALIGN_ATTRIBUTE_NAME             "align"
//#define EZCFG_HTML_ALINK_ATTRIBUTE_NAME             "alink"
#define EZCFG_HTML_ALT_ATTRIBUTE_NAME               "alt"
#define EZCFG_HTML_ARCHIVE_ATTRIBUTE_NAME           "archive"
#define EZCFG_HTML_AXIS_ATTRIBUTE_NAME              "axis"
//#define EZCFG_HTML_BACKGROUND_ATTRIBUTE_NAME        "background"
//#define EZCFG_HTML_BGCOLOR_ATTRIBUTE_NAME           "bgcolor"
#define EZCFG_HTML_BORDER_ATTRIBUTE_NAME            "border"
#define EZCFG_HTML_CELLPADDING_ATTRIBUTE_NAME       "cellpadding"
#define EZCFG_HTML_CELLSPACING_ATTRIBUTE_NAME       "cellspacing"
#define EZCFG_HTML_CHAR_ATTRIBUTE_NAME              "char"
#define EZCFG_HTML_CHAROFF_ATTRIBUTE_NAME           "charoff"
#define EZCFG_HTML_CHARSET_ATTRIBUTE_NAME           "charset"
#define EZCFG_HTML_CHECKED_ATTRIBUTE_NAME           "checked"
#define EZCFG_HTML_CITE_ATTRIBUTE_NAME              "cite"
#define EZCFG_HTML_CLASS_ATTRIBUTE_NAME             "class"
#define EZCFG_HTML_CLASSID_ATTRIBUTE_NAME           "classid"
//#define EZCFG_HTML_CLEAR_ATTRIBUTE_NAME             "clear"
//#define EZCFG_HTML_CODE_ATTRIBUTE_NAME              "code"
#define EZCFG_HTML_CODEBASE_ATTRIBUTE_NAME          "codebase"
#define EZCFG_HTML_CODETYPE_ATTRIBUTE_NAME          "codetype"
//#define EZCFG_HTML_COLOR_ATTRIBUTE_NAME             "color"
#define EZCFG_HTML_COLS_ATTRIBUTE_NAME              "cols"
#define EZCFG_HTML_COLSPAN_ATTRIBUTE_NAME           "colspan"
//#define EZCFG_HTML_COMPACT_ATTRIBUTE_NAME           "compact"
#define EZCFG_HTML_CONTENT_ATTRIBUTE_NAME           "content"
#define EZCFG_HTML_COORDS_ATTRIBUTE_NAME            "coords"
#define EZCFG_HTML_DATA_ATTRIBUTE_NAME              "data"
#define EZCFG_HTML_DATETIME_ATTRIBUTE_NAME          "datetime"
#define EZCFG_HTML_DECLARE_ATTRIBUTE_NAME           "declare"
#define EZCFG_HTML_DEFER_ATTRIBUTE_NAME             "defer"
#define EZCFG_HTML_DIR_ATTRIBUTE_NAME               "dir"
#define EZCFG_HTML_DISABLED_ATTRIBUTE_NAME          "disabled"
#define EZCFG_HTML_ENCTYPE_ATTRIBUTE_NAME           "enctype"
//#define EZCFG_HTML_FACE_ATTRIBUTE_NAME              "face"
#define EZCFG_HTML_FOR_ATTRIBUTE_NAME               "for"
#define EZCFG_HTML_FRAME_ATTRIBUTE_NAME             "frame"
//#define EZCFG_HTML_FRAMEBORDER_ATTRIBUTE_NAME       "frameborder"
#define EZCFG_HTML_HEADERS_ATTRIBUTE_NAME           "headers"
#define EZCFG_HTML_HEIGHT_ATTRIBUTE_NAME            "height"
#define EZCFG_HTML_HREF_ATTRIBUTE_NAME              "href"
#define EZCFG_HTML_HREFLANG_ATTRIBUTE_NAME          "hreflang"
//#define EZCFG_HTML_HSPACE_ATTRIBUTE_NAME            "hspace"
#define EZCFG_HTML_HTTP_EQUIV_ATTRIBUTE_NAME        "http-equiv"
#define EZCFG_HTML_ID_ATTRIBUTE_NAME                "id"
#define EZCFG_HTML_ISMAP_ATTRIBUTE_NAME             "ismap"
#define EZCFG_HTML_LABEL_ATTRIBUTE_NAME             "label"
#define EZCFG_HTML_LANG_ATTRIBUTE_NAME              "lang"
//#define EZCFG_HTML_LANGUAGE_ATTRIBUTE_NAME          "language"
//#define EZCFG_HTML_LINK_ATTRIBUTE_NAME              "link"
#define EZCFG_HTML_LONGDESC_ATTRIBUTE_NAME          "longdesc"
//#define EZCFG_HTML_MARGINHEIGHT_ATTRIBUTE_NAME      "marginheight"
//#define EZCFG_HTML_MARGINWIDTH_ATTRIBUTE_NAME       "marginwidth"
#define EZCFG_HTML_MAXLENGTH_ATTRIBUTE_NAME         "maxlength"
#define EZCFG_HTML_MEDIA_ATTRIBUTE_NAME             "media"
#define EZCFG_HTML_METHOD_ATTRIBUTE_NAME            "method"
#define EZCFG_HTML_MULTIPLE_ATTRIBUTE_NAME          "multiple"
#define EZCFG_HTML_NAME_ATTRIBUTE_NAME              "name"
#define EZCFG_HTML_NOHREF_ATTRIBUTE_NAME            "nohref"
//#define EZCFG_HTML_NORESIZE_ATTRIBUTE_NAME          "noresize"
//#define EZCFG_HTML_NOSHADE_ATTRIBUTE_NAME           "noshade"
//#define EZCFG_HTML_NOWRAP_ATTRIBUTE_NAME            "nowrap"
//#define EZCFG_HTML_OBJECT_ATTRIBUTE_NAME            "object"
#define EZCFG_HTML_ONBLUR_ATTRIBUTE_NAME            "onblur"
#define EZCFG_HTML_ONCHANGE_ATTRIBUTE_NAME          "onchange"
#define EZCFG_HTML_ONCLICK_ATTRIBUTE_NAME           "onclick"
#define EZCFG_HTML_ONDBLCLICK_ATTRIBUTE_NAME        "ondblclick"
#define EZCFG_HTML_ONFOCUS_ATTRIBUTE_NAME           "onfocus"
#define EZCFG_HTML_ONKEYDOWN_ATTRIBUTE_NAME         "onkeydown"
#define EZCFG_HTML_ONKEYPRESS_ATTRIBUTE_NAME        "onkeypress"
#define EZCFG_HTML_ONKEYUP_ATTRIBUTE_NAME           "onkeyup"
#define EZCFG_HTML_ONLOAD_ATTRIBUTE_NAME            "onload"
#define EZCFG_HTML_ONMOUSEDOWN_ATTRIBUTE_NAME       "onmousedown"
#define EZCFG_HTML_ONMOUSEMOVE_ATTRIBUTE_NAME       "onmousemove"
#define EZCFG_HTML_ONMOUSEOUT_ATTRIBUTE_NAME        "onmouseout"
#define EZCFG_HTML_ONMOUSEOVER_ATTRIBUTE_NAME       "onmouseover"
#define EZCFG_HTML_ONMOUSEUP_ATTRIBUTE_NAME         "onmouseup"
#define EZCFG_HTML_ONRESET_ATTRIBUTE_NAME           "onreset"
#define EZCFG_HTML_ONSELECT_ATTRIBUTE_NAME          "onselect"
#define EZCFG_HTML_ONSUBMIT_ATTRIBUTE_NAME          "onsubmit"
#define EZCFG_HTML_ONUNLOAD_ATTRIBUTE_NAME          "onunload"
#define EZCFG_HTML_PROFILE_ATTRIBUTE_NAME           "profile"
//#define EZCFG_HTML_PROMPT_ATTRIBUTE_NAME            "prompt"
#define EZCFG_HTML_READONLY_ATTRIBUTE_NAME          "readonly"
#define EZCFG_HTML_REL_ATTRIBUTE_NAME               "rel"
#define EZCFG_HTML_REV_ATTRIBUTE_NAME               "rev"
#define EZCFG_HTML_ROWS_ATTRIBUTE_NAME              "rows"
#define EZCFG_HTML_ROWSPAN_ATTRIBUTE_NAME           "rowspan"
#define EZCFG_HTML_RULES_ATTRIBUTE_NAME             "rules"
#define EZCFG_HTML_SCHEME_ATTRIBUTE_NAME            "scheme"
#define EZCFG_HTML_SCOPE_ATTRIBUTE_NAME             "scope"
//#define EZCFG_HTML_SCROLLING_ATTRIBUTE_NAME         "scrolling"
#define EZCFG_HTML_SELECTED_ATTRIBUTE_NAME          "selected"
#define EZCFG_HTML_SHAPE_ATTRIBUTE_NAME             "shape"
#define EZCFG_HTML_SIZE_ATTRIBUTE_NAME              "size"
#define EZCFG_HTML_SPAN_ATTRIBUTE_NAME              "span"
#define EZCFG_HTML_SRC_ATTRIBUTE_NAME               "src"
#define EZCFG_HTML_STANDBY_ATTRIBUTE_NAME           "standby"
//#define EZCFG_HTML_START_ATTRIBUTE_NAME             "start"
#define EZCFG_HTML_STYLE_ATTRIBUTE_NAME             "style"
#define EZCFG_HTML_SUMMARY_ATTRIBUTE_NAME           "summary"
#define EZCFG_HTML_TABINDEX_ATTRIBUTE_NAME          "tabindex"
//#define EZCFG_HTML_TARGET_ATTRIBUTE_NAME            "target"
//#define EZCFG_HTML_TEXT_ATTRIBUTE_NAME              "text"
#define EZCFG_HTML_TITLE_ATTRIBUTE_NAME             "title"
#define EZCFG_HTML_TYPE_ATTRIBUTE_NAME              "type"
#define EZCFG_HTML_USEMAP_ATTRIBUTE_NAME            "usemap"
#define EZCFG_HTML_VALIGN_ATTRIBUTE_NAME            "valign"
#define EZCFG_HTML_VALUE_ATTRIBUTE_NAME             "value"
#define EZCFG_HTML_VALUETYPE_ATTRIBUTE_NAME         "valuetype"
//#define EZCFG_HTML_VERSION_ATTRIBUTE_NAME           "version"
//#define EZCFG_HTML_VLINK_ATTRIBUTE_NAME             "vlink"
//#define EZCFG_HTML_VSPACE_ATTRIBUTE_NAME            "vspace"
#define EZCFG_HTML_WIDTH_ATTRIBUTE_NAME             "width"

#endif /* _EZCFG_HTML_H_ */

