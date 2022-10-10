struct qoi_header {
    char magix[4];
    be u32 width;
    be u32 height;
    u8 n_channels;
    u8 colourspace;
};

struct qoi_rgb {
    u8 tag;
    u8 r;
    u8 g;
    u8 b;
};

struct qoi_rgba {
    u8 tag;
    u8 r;
    u8 g;
    u8 b;
    u8 a;
};

bitfield qoi_index {
    tag : 2;
    index : 6;
};

bitfield qoi_diff {
    tag : 2;
    dr : 2;
    dg : 2;
    db : 2;
};

bitfield qoi_luma {
    tag : 2;
    dg : 6;
    dr_dg : 4;
    db_dg : 4;
};

bitfield qoi_run {
    tag : 2;
    run : 6;
};

struct qoi_chunk {
    u8 tag [[no_unique_address]];
    if (tag == 0xfe) {
        qoi_rgb rgb;
    } else if (tag == 0xff) {
        qoi_rgba rgba;
    } else if ((tag >> 6) == 0b00) {
        qoi_index index;
    } else if ((tag >> 6) == 0b01) {
        qoi_diff diff;
    } else if ((tag >> 6) == 0b10) {
        qoi_luma luma;
    } else if ((tag >> 6) == 0b11) {
        qoi_run run;
    }

};

struct qoi_file {
    qoi_header header;
    qoi_chunk chunks[2000];
};

qoi_file file @ 0x00;