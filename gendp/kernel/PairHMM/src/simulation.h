#include "sys_def.h"

class comp_decoder {

    public:

        comp_decoder();
        ~comp_decoder();

        void execute(long instruction, int* op, int* in_addr, int* out_addr, int* PC);

    private:

};

class alu_32 {

	public:

		alu_32();
		~alu_32();

        int execute(int input_0, int input_1, int op);
        int execute_8bit(int input_0, int input_1, int op);

        int execute_4input(int input_0, int input_1, int input_2, int input_3, int op);
        int execute_4input_8bit(int input_0, int input_1, int input_2, int input_3, int op);

	private:

        char LogTable256[256] = {
			#define LT(n) n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n
			0, 0, 1, 1, 2, 2, 2, 2, 3, 3, 3, 3, 3, 3, 3, 3,
			LT(4), LT(5), LT(5), LT(6), LT(6), LT(6), LT(6),
			LT(7), LT(7), LT(7), LT(7), LT(7), LT(7), LT(7), LT(7)
		};

		int ilog2_32(unsigned int v);

        int logSumTable[17];

        void logSumTable_init();

		int8_t input_0_simd[SIMD_WIDTH8], input_1_simd[SIMD_WIDTH8], input_3_simd[SIMD_WIDTH8], input_2_simd[SIMD_WIDTH8], output_simd[SIMD_WIDTH8];
};

class compute_unit_32 {

	public:

		compute_unit_32();
		~compute_unit_32();

		int execute(int* op, int* input);
		int execute_8bit(int* op, int* input);

	private:

		int match_score_poa[5][5] = {{7, -1,  0,  0, -3},
									{-1, 13, -6, -2, -5},
									{ 0, -6,  9, -3, -1},
									{ 0, -2, -3,  8,  0},
									{-3, -5, -1,  0,  9}};

		int match_score_phmm[5][5] = {{1,-1, -1, -1,  1},
									{-1,  1, -1, -1,  1},
									{-1, -1,  1, -1,  1},
									{-1, -1, -1,  1,  1},
									{ 1,  1,  1,  1,  1}};

		int8_t match_score_bsw_scalar[5][5] = {{1, -4, -4, -4, -1},
									{-4,  1, -4, -4, -1},
									{-4, -4,  1, -4, -1},
									{-4, -4, -4,  1, -1},
									{-1, -1, -1, -1, -1}};

		int QUAL2PROB_TABLE[256];
		int QUAL2ERROR_DIV3_TABLE[256];

		void LUT_init();
		// int8_t match_score_bsw_simd[5][5][SIMD_WIDTH8];
		// int match_score_bsw_32[5][5];

		alu_32 alu;
		
		int8_t input_0_simd[SIMD_WIDTH8], input_1_simd[SIMD_WIDTH8], input_2_simd[SIMD_WIDTH8], input_3_simd[SIMD_WIDTH8], input_4_simd[SIMD_WIDTH8], input_5_simd[SIMD_WIDTH8], alu_out_1_simd[SIMD_WIDTH8], output_simd[SIMD_WIDTH8];

};

compute_unit_32::compute_unit_32() {
    LUT_init();
	// int i, j, k;
	// for (i=0; i<5; i++) {
	// 	for (j=0; j<5; j++) {
	// 		for (k=0; k<SIMD_WIDTH8; k++) {
	// 			match_score_bsw_simd[i][j][k] = match_score_bsw_scalar[i][j];
	// 		}
	// 		memcpy(&match_score_bsw_32[i][j], match_score_bsw_simd[i][j], SIMD_WIDTH8*sizeof(int8_t));
	// 	}
	// }
}
compute_unit_32::~compute_unit_32() {}

int compute_unit_32::execute_8bit(int* op, int* input) {

	int alu_out_0, alu_out_1, alu_out_2, out = 0, zero = 0;
	int input_0, input_1, input_2, input_3, input_4, input_5, op_0, op_1, op_2;
	input_0 = input[0];
	input_1 = input[1];
	input_2 = input[2];
	input_3 = input[3];
	input_4 = input[4];
	input_5 = input[5];
	op_0 = op[0];
	op_1 = op[1];
	op_2 = op[2];

	alu_out_0 = alu.execute_4input_8bit(input_0, input_1, input_2, input_3, op_0);
	alu_out_1 = alu.execute_8bit(input_4, input_5, op_1);
	alu_out_2 = alu.execute_8bit(alu_out_0, alu_out_1, op_2);

	// int8_t *input_0_simd, *input_1_simd, *alu_out_1_simd, *output_simd;
	// input_0_simd = (int8_t*) malloc (SIMD_WIDTH8 * sizeof(int8_t));
	// input_1_simd = (int8_t*) malloc (SIMD_WIDTH8 * sizeof(int8_t));
	// alu_out_1_simd = (int8_t*) malloc (SIMD_WIDTH8 * sizeof(int8_t));
	// output_simd = (int8_t*) malloc (SIMD_WIDTH8 * sizeof(int8_t));
	// int8_t input_0_simd[SIMD_WIDTH8], input_1_simd[SIMD_WIDTH8], input_2_simd[SIMD_WIDTH8], input_3_simd[SIMD_WIDTH8], input_4_simd[SIMD_WIDTH8], input_5_simd[SIMD_WIDTH8], alu_out_1_simd[SIMD_WIDTH8], output_simd[SIMD_WIDTH8];
	memcpy(output_simd, &zero, SIMD_WIDTH8 * sizeof(int8_t));
	memcpy(input_0_simd, &input_0, SIMD_WIDTH8 * sizeof(int8_t));
	memcpy(input_1_simd, &input_1, SIMD_WIDTH8 * sizeof(int8_t));
	memcpy(input_2_simd, &input_2, SIMD_WIDTH8 * sizeof(int8_t));
	memcpy(input_3_simd, &input_3, SIMD_WIDTH8 * sizeof(int8_t));
	memcpy(input_4_simd, &input_4, SIMD_WIDTH8 * sizeof(int8_t));
	memcpy(input_5_simd, &input_5, SIMD_WIDTH8 * sizeof(int8_t));
	memcpy(alu_out_1_simd, &alu_out_1, SIMD_WIDTH8 * sizeof(int8_t));
	int i;

	if (op_0 == 10) {
		for (i=0; i<SIMD_WIDTH8; i++) {
			if (input_0_simd[i] > 4 || input_1_simd[i] > 4) output_simd[i] = 0;
			else output_simd[i] = match_score_bsw_scalar[input_0_simd[i]][input_1_simd[i]] + alu_out_1_simd[i];
		}
		memcpy(&out, output_simd, SIMD_WIDTH8 * sizeof(int8_t));
	} else out = alu_out_2;
	memcpy(output_simd, &out, SIMD_WIDTH8 * sizeof(int8_t));

#ifdef PROFILE
	for (i=0; i<SIMD_WIDTH8; i++)
		printf("%d %d %d %d %d %d %d %d %d %d\n", op_0, op_1, op_2, input_0_simd[i], input_1_simd[i], input_2_simd[i], input_3_simd[i], input_4_simd[i], input_5_simd[i], output_simd[i]);
#endif

	return out;
}

int compute_unit_32::execute(int* op, int* input) {
	
	int alu_out_0, alu_out_1, alu_out_2, out = 0;
	int input_0, input_1, input_2, input_3, input_4, input_5, op_0, op_1, op_2;
	// long mul_tmp;
	input_0 = input[0];
	input_1 = input[1];
	input_2 = input[2];
	input_3 = input[3];
	input_4 = input[4];
	input_5 = input[5];
	op_0 = op[0];
	op_1 = op[1];
	op_2 = op[2];

	alu_out_0 = alu.execute_4input(input_0, input_1, input_2, input_3, op_0);
	alu_out_1 = alu.execute(input_4, input_5, op_1);
	alu_out_2 = alu.execute(alu_out_0, alu_out_1, op_2);

	// for (int i = 1; i < 5; i++) {
	// 	for (int j = 1; j < 5; j++)
	// 	printf("%d ", match_score_poa[i][j]);
	// 	printf("\n");
	// }

	// for (int i = 0; i < 25; i++) printf("%d ", **(match_score_poa + i));


	if (op_0 == 2) {
		// mul_tmp = (long)input_0 * (long)input_1;
		// out = (int)(mul_tmp >> 32);
		out = input_0 * input_1;
	} else if (op_0 == 10) {
		if ((input_0 > 4) || (input_1 > 4)) alu_out_0 = 0;
		else if (op_2 == 1) out = match_score_poa[input_0][input_1] + alu_out_1;
		else if (op_2 == 2) {
            // printf("%d %d %d %d\n", match_score_phmm[input_0][input_1], input_2, QUAL2PROB_TABLE[input_2], QUAL2ERROR_DIV3_TABLE[input_2]);
            out = (match_score_phmm[input_0][input_1] == 1 ? QUAL2PROB_TABLE[input_2] : QUAL2ERROR_DIV3_TABLE[input_2]);
            // printf("%d\n", out);
        }
		else out = 0;
	} else out = alu_out_2;

	// printf("%d %d %d %d %d %d %d %d %d %d\n", op_0, op_1, op_2, input_0, input_1, input_2, input_3, input_4, input_5, out);
#ifdef PROFILE
	printf("%d %d %d %d %d %d %d %d %d %d\t", op_0, op_1, op_2, input_0, input_1, input_2, input_3, input_4, input_5, out);
	// printf("%d %d %d %d\n", alu_out_0, alu_out_1, alu_out_2, out);
#endif
	return out;
}

void compute_unit_32::LUT_init() {

        QUAL2PROB_TABLE[0] = 0;
        QUAL2PROB_TABLE[1] = -149525;
        QUAL2PROB_TABLE[2] = -94249;
        QUAL2PROB_TABLE[3] = -65760;
        QUAL2PROB_TABLE[4] = -47999;
        QUAL2PROB_TABLE[5] = -35940;
        QUAL2PROB_TABLE[6] = -27349;
        QUAL2PROB_TABLE[7] = -21041;
        QUAL2PROB_TABLE[8] = -16314;
        QUAL2PROB_TABLE[9] = -12721;
        QUAL2PROB_TABLE[10] = -9961;
        QUAL2PROB_TABLE[11] = -7825;
        QUAL2PROB_TABLE[12] = -6162;
        QUAL2PROB_TABLE[13] = -4861;
        QUAL2PROB_TABLE[14] = -3841;
        QUAL2PROB_TABLE[15] = -3038;
        QUAL2PROB_TABLE[16] = -2405;
        QUAL2PROB_TABLE[17] = -1905;
        QUAL2PROB_TABLE[18] = -1510;
        QUAL2PROB_TABLE[19] = -1197;
        QUAL2PROB_TABLE[20] = -950;
        QUAL2PROB_TABLE[21] = -754;
        QUAL2PROB_TABLE[22] = -598;
        QUAL2PROB_TABLE[23] = -475;
        QUAL2PROB_TABLE[24] = -377;
        QUAL2PROB_TABLE[25] = -299;
        QUAL2PROB_TABLE[26] = -237;
        QUAL2PROB_TABLE[27] = -188;
        QUAL2PROB_TABLE[28] = -149;
        QUAL2PROB_TABLE[29] = -119;
        QUAL2PROB_TABLE[30] = -94;
        QUAL2PROB_TABLE[31] = -75;
        QUAL2PROB_TABLE[32] = -59;
        QUAL2PROB_TABLE[33] = -47;
        QUAL2PROB_TABLE[34] = -37;
        QUAL2PROB_TABLE[35] = -29;
        QUAL2PROB_TABLE[36] = -23;
        QUAL2PROB_TABLE[37] = -18;
        QUAL2PROB_TABLE[38] = -14;
        QUAL2PROB_TABLE[39] = -11;
        QUAL2PROB_TABLE[40] = -9;
        QUAL2PROB_TABLE[41] = -7;
        QUAL2PROB_TABLE[42] = -5;
        QUAL2PROB_TABLE[43] = -4;
        QUAL2PROB_TABLE[44] = -3;
        QUAL2PROB_TABLE[45] = -2;
        QUAL2PROB_TABLE[46] = -2;
        QUAL2PROB_TABLE[47] = -1;
        QUAL2PROB_TABLE[48] = -1;
        QUAL2PROB_TABLE[49] = -1;
        QUAL2PROB_TABLE[50] = 0;
        QUAL2PROB_TABLE[51] = 0;
        QUAL2PROB_TABLE[52] = 0;
        QUAL2PROB_TABLE[53] = 0;
        QUAL2PROB_TABLE[54] = 0;
        QUAL2PROB_TABLE[55] = 0;
        QUAL2PROB_TABLE[56] = 0;
        QUAL2PROB_TABLE[57] = 0;
        QUAL2PROB_TABLE[58] = 0;
        QUAL2PROB_TABLE[59] = 0;
        QUAL2PROB_TABLE[60] = 0;
        QUAL2PROB_TABLE[61] = 0;
        QUAL2PROB_TABLE[62] = 0;
        QUAL2PROB_TABLE[63] = 0;
        QUAL2PROB_TABLE[64] = 0;
        QUAL2PROB_TABLE[65] = 0;
        QUAL2PROB_TABLE[66] = 0;
        QUAL2PROB_TABLE[67] = 0;
        QUAL2PROB_TABLE[68] = 0;
        QUAL2PROB_TABLE[69] = 0;
        QUAL2PROB_TABLE[70] = 0;
        QUAL2PROB_TABLE[71] = 0;
        QUAL2PROB_TABLE[72] = 0;
        QUAL2PROB_TABLE[73] = 0;
        QUAL2PROB_TABLE[74] = 0;
        QUAL2PROB_TABLE[75] = 0;
        QUAL2PROB_TABLE[76] = 0;
        QUAL2PROB_TABLE[77] = 0;
        QUAL2PROB_TABLE[78] = 0;
        QUAL2PROB_TABLE[79] = 0;
        QUAL2PROB_TABLE[80] = 0;
        QUAL2PROB_TABLE[81] = 0;
        QUAL2PROB_TABLE[82] = 0;
        QUAL2PROB_TABLE[83] = 0;
        QUAL2PROB_TABLE[84] = 0;
        QUAL2PROB_TABLE[85] = 0;
        QUAL2PROB_TABLE[86] = 0;
        QUAL2PROB_TABLE[87] = 0;
        QUAL2PROB_TABLE[88] = 0;
        QUAL2PROB_TABLE[89] = 0;
        QUAL2PROB_TABLE[90] = 0;
        QUAL2PROB_TABLE[91] = 0;
        QUAL2PROB_TABLE[92] = 0;
        QUAL2PROB_TABLE[93] = 0;
        QUAL2PROB_TABLE[94] = 0;
        QUAL2PROB_TABLE[95] = 0;
        QUAL2PROB_TABLE[96] = 0;
        QUAL2PROB_TABLE[97] = 0;
        QUAL2PROB_TABLE[98] = 0;
        QUAL2PROB_TABLE[99] = 0;
        QUAL2PROB_TABLE[100] = 0;
        QUAL2PROB_TABLE[101] = 0;
        QUAL2PROB_TABLE[102] = 0;
        QUAL2PROB_TABLE[103] = 0;
        QUAL2PROB_TABLE[104] = 0;
        QUAL2PROB_TABLE[105] = 0;
        QUAL2PROB_TABLE[106] = 0;
        QUAL2PROB_TABLE[107] = 0;
        QUAL2PROB_TABLE[108] = 0;
        QUAL2PROB_TABLE[109] = 0;
        QUAL2PROB_TABLE[110] = 0;
        QUAL2PROB_TABLE[111] = 0;
        QUAL2PROB_TABLE[112] = 0;
        QUAL2PROB_TABLE[113] = 0;
        QUAL2PROB_TABLE[114] = 0;
        QUAL2PROB_TABLE[115] = 0;
        QUAL2PROB_TABLE[116] = 0;
        QUAL2PROB_TABLE[117] = 0;
        QUAL2PROB_TABLE[118] = 0;
        QUAL2PROB_TABLE[119] = 0;
        QUAL2PROB_TABLE[120] = 0;
        QUAL2PROB_TABLE[121] = 0;
        QUAL2PROB_TABLE[122] = 0;
        QUAL2PROB_TABLE[123] = 0;
        QUAL2PROB_TABLE[124] = 0;
        QUAL2PROB_TABLE[125] = 0;
        QUAL2PROB_TABLE[126] = 0;
        QUAL2PROB_TABLE[127] = 0;
        QUAL2PROB_TABLE[128] = 0;
        QUAL2PROB_TABLE[129] = 0;
        QUAL2PROB_TABLE[130] = 0;
        QUAL2PROB_TABLE[131] = 0;
        QUAL2PROB_TABLE[132] = 0;
        QUAL2PROB_TABLE[133] = 0;
        QUAL2PROB_TABLE[134] = 0;
        QUAL2PROB_TABLE[135] = 0;
        QUAL2PROB_TABLE[136] = 0;
        QUAL2PROB_TABLE[137] = 0;
        QUAL2PROB_TABLE[138] = 0;
        QUAL2PROB_TABLE[139] = 0;
        QUAL2PROB_TABLE[140] = 0;
        QUAL2PROB_TABLE[141] = 0;
        QUAL2PROB_TABLE[142] = 0;
        QUAL2PROB_TABLE[143] = 0;
        QUAL2PROB_TABLE[144] = 0;
        QUAL2PROB_TABLE[145] = 0;
        QUAL2PROB_TABLE[146] = 0;
        QUAL2PROB_TABLE[147] = 0;
        QUAL2PROB_TABLE[148] = 0;
        QUAL2PROB_TABLE[149] = 0;
        QUAL2PROB_TABLE[150] = 0;
        QUAL2PROB_TABLE[151] = 0;
        QUAL2PROB_TABLE[152] = 0;
        QUAL2PROB_TABLE[153] = 0;
        QUAL2PROB_TABLE[154] = 0;
        QUAL2PROB_TABLE[155] = 0;
        QUAL2PROB_TABLE[156] = 0;
        QUAL2PROB_TABLE[157] = 0;
        QUAL2PROB_TABLE[158] = 0;
        QUAL2PROB_TABLE[159] = 0;
        QUAL2PROB_TABLE[160] = 0;
        QUAL2PROB_TABLE[161] = 0;
        QUAL2PROB_TABLE[162] = 0;
        QUAL2PROB_TABLE[163] = 0;
        QUAL2PROB_TABLE[164] = 0;
        QUAL2PROB_TABLE[165] = 0;
        QUAL2PROB_TABLE[166] = 0;
        QUAL2PROB_TABLE[167] = 0;
        QUAL2PROB_TABLE[168] = 0;
        QUAL2PROB_TABLE[169] = 0;
        QUAL2PROB_TABLE[170] = 0;
        QUAL2PROB_TABLE[171] = 0;
        QUAL2PROB_TABLE[172] = 0;
        QUAL2PROB_TABLE[173] = 0;
        QUAL2PROB_TABLE[174] = 0;
        QUAL2PROB_TABLE[175] = 0;
        QUAL2PROB_TABLE[176] = 0;
        QUAL2PROB_TABLE[177] = 0;
        QUAL2PROB_TABLE[178] = 0;
        QUAL2PROB_TABLE[179] = 0;
        QUAL2PROB_TABLE[180] = 0;
        QUAL2PROB_TABLE[181] = 0;
        QUAL2PROB_TABLE[182] = 0;
        QUAL2PROB_TABLE[183] = 0;
        QUAL2PROB_TABLE[184] = 0;
        QUAL2PROB_TABLE[185] = 0;
        QUAL2PROB_TABLE[186] = 0;
        QUAL2PROB_TABLE[187] = 0;
        QUAL2PROB_TABLE[188] = 0;
        QUAL2PROB_TABLE[189] = 0;
        QUAL2PROB_TABLE[190] = 0;
        QUAL2PROB_TABLE[191] = 0;
        QUAL2PROB_TABLE[192] = 0;
        QUAL2PROB_TABLE[193] = 0;
        QUAL2PROB_TABLE[194] = 0;
        QUAL2PROB_TABLE[195] = 0;
        QUAL2PROB_TABLE[196] = 0;
        QUAL2PROB_TABLE[197] = 0;
        QUAL2PROB_TABLE[198] = 0;
        QUAL2PROB_TABLE[199] = 0;
        QUAL2PROB_TABLE[200] = 0;
        QUAL2PROB_TABLE[201] = 0;
        QUAL2PROB_TABLE[202] = 0;
        QUAL2PROB_TABLE[203] = 0;
        QUAL2PROB_TABLE[204] = 0;
        QUAL2PROB_TABLE[205] = 0;
        QUAL2PROB_TABLE[206] = 0;
        QUAL2PROB_TABLE[207] = 0;
        QUAL2PROB_TABLE[208] = 0;
        QUAL2PROB_TABLE[209] = 0;
        QUAL2PROB_TABLE[210] = 0;
        QUAL2PROB_TABLE[211] = 0;
        QUAL2PROB_TABLE[212] = 0;
        QUAL2PROB_TABLE[213] = 0;
        QUAL2PROB_TABLE[214] = 0;
        QUAL2PROB_TABLE[215] = 0;
        QUAL2PROB_TABLE[216] = 0;
        QUAL2PROB_TABLE[217] = 0;
        QUAL2PROB_TABLE[218] = 0;
        QUAL2PROB_TABLE[219] = 0;
        QUAL2PROB_TABLE[220] = 0;
        QUAL2PROB_TABLE[221] = 0;
        QUAL2PROB_TABLE[222] = 0;
        QUAL2PROB_TABLE[223] = 0;
        QUAL2PROB_TABLE[224] = 0;
        QUAL2PROB_TABLE[225] = 0;
        QUAL2PROB_TABLE[226] = 0;
        QUAL2PROB_TABLE[227] = 0;
        QUAL2PROB_TABLE[228] = 0;
        QUAL2PROB_TABLE[229] = 0;
        QUAL2PROB_TABLE[230] = 0;
        QUAL2PROB_TABLE[231] = 0;
        QUAL2PROB_TABLE[232] = 0;
        QUAL2PROB_TABLE[233] = 0;
        QUAL2PROB_TABLE[234] = 0;
        QUAL2PROB_TABLE[235] = 0;
        QUAL2PROB_TABLE[236] = 0;
        QUAL2PROB_TABLE[237] = 0;
        QUAL2PROB_TABLE[238] = 0;
        QUAL2PROB_TABLE[239] = 0;
        QUAL2PROB_TABLE[240] = 0;
        QUAL2PROB_TABLE[241] = 0;
        QUAL2PROB_TABLE[242] = 0;
        QUAL2PROB_TABLE[243] = 0;
        QUAL2PROB_TABLE[244] = 0;
        QUAL2PROB_TABLE[245] = 0;
        QUAL2PROB_TABLE[246] = 0;
        QUAL2PROB_TABLE[247] = 0;
        QUAL2PROB_TABLE[248] = 0;
        QUAL2PROB_TABLE[249] = 0;
        QUAL2PROB_TABLE[250] = 0;
        QUAL2PROB_TABLE[251] = 0;
        QUAL2PROB_TABLE[252] = 0;
        QUAL2PROB_TABLE[253] = 0;
        QUAL2PROB_TABLE[254] = 0;
        QUAL2PROB_TABLE[255] = 0;

        QUAL2ERROR_DIV3_TABLE[0] = -103872;
        QUAL2ERROR_DIV3_TABLE[1] = -125642;
        QUAL2ERROR_DIV3_TABLE[2] = -147413;
        QUAL2ERROR_DIV3_TABLE[3] = -169183;
        QUAL2ERROR_DIV3_TABLE[4] = -190954;
        QUAL2ERROR_DIV3_TABLE[5] = -212725;
        QUAL2ERROR_DIV3_TABLE[6] = -234495;
        QUAL2ERROR_DIV3_TABLE[7] = -256266;
        QUAL2ERROR_DIV3_TABLE[8] = -278036;
        QUAL2ERROR_DIV3_TABLE[9] = -299807;
        QUAL2ERROR_DIV3_TABLE[10] = -321577;
        QUAL2ERROR_DIV3_TABLE[11] = -343348;
        QUAL2ERROR_DIV3_TABLE[12] = -365119;
        QUAL2ERROR_DIV3_TABLE[13] = -386889;
        QUAL2ERROR_DIV3_TABLE[14] = -408660;
        QUAL2ERROR_DIV3_TABLE[15] = -430430;
        QUAL2ERROR_DIV3_TABLE[16] = -452201;
        QUAL2ERROR_DIV3_TABLE[17] = -473972;
        QUAL2ERROR_DIV3_TABLE[18] = -495742;
        QUAL2ERROR_DIV3_TABLE[19] = -517513;
        QUAL2ERROR_DIV3_TABLE[20] = -539283;
        QUAL2ERROR_DIV3_TABLE[21] = -561054;
        QUAL2ERROR_DIV3_TABLE[22] = -582825;
        QUAL2ERROR_DIV3_TABLE[23] = -604595;
        QUAL2ERROR_DIV3_TABLE[24] = -626366;
        QUAL2ERROR_DIV3_TABLE[25] = -648136;
        QUAL2ERROR_DIV3_TABLE[26] = -669907;
        QUAL2ERROR_DIV3_TABLE[27] = -691678;
        QUAL2ERROR_DIV3_TABLE[28] = -713448;
        QUAL2ERROR_DIV3_TABLE[29] = -735219;
        QUAL2ERROR_DIV3_TABLE[30] = -756989;
        QUAL2ERROR_DIV3_TABLE[31] = -778760;
        QUAL2ERROR_DIV3_TABLE[32] = -800530;
        QUAL2ERROR_DIV3_TABLE[33] = -822301;
        QUAL2ERROR_DIV3_TABLE[34] = -844072;
        QUAL2ERROR_DIV3_TABLE[35] = -865842;
        QUAL2ERROR_DIV3_TABLE[36] = -887613;
        QUAL2ERROR_DIV3_TABLE[37] = -909383;
        QUAL2ERROR_DIV3_TABLE[38] = -931154;
        QUAL2ERROR_DIV3_TABLE[39] = -952925;
        QUAL2ERROR_DIV3_TABLE[40] = -974695;
        QUAL2ERROR_DIV3_TABLE[41] = -996466;
        QUAL2ERROR_DIV3_TABLE[42] = -1018236;
        QUAL2ERROR_DIV3_TABLE[43] = -1040007;
        QUAL2ERROR_DIV3_TABLE[44] = -1061778;
        QUAL2ERROR_DIV3_TABLE[45] = -1083548;
        QUAL2ERROR_DIV3_TABLE[46] = -1105319;
        QUAL2ERROR_DIV3_TABLE[47] = -1127089;
        QUAL2ERROR_DIV3_TABLE[48] = -1148860;
        QUAL2ERROR_DIV3_TABLE[49] = -1170630;
        QUAL2ERROR_DIV3_TABLE[50] = -1192401;
        QUAL2ERROR_DIV3_TABLE[51] = -1214172;
        QUAL2ERROR_DIV3_TABLE[52] = -1235942;
        QUAL2ERROR_DIV3_TABLE[53] = -1257713;
        QUAL2ERROR_DIV3_TABLE[54] = -1279483;
        QUAL2ERROR_DIV3_TABLE[55] = -1301254;
        QUAL2ERROR_DIV3_TABLE[56] = -1323025;
        QUAL2ERROR_DIV3_TABLE[57] = -1344795;
        QUAL2ERROR_DIV3_TABLE[58] = -1366566;
        QUAL2ERROR_DIV3_TABLE[59] = -1388336;
        QUAL2ERROR_DIV3_TABLE[60] = -1410107;
        QUAL2ERROR_DIV3_TABLE[61] = -1431878;
        QUAL2ERROR_DIV3_TABLE[62] = -1453648;
        QUAL2ERROR_DIV3_TABLE[63] = -1475419;
        QUAL2ERROR_DIV3_TABLE[64] = -1475419;
        QUAL2ERROR_DIV3_TABLE[65] = -1475419;
        QUAL2ERROR_DIV3_TABLE[66] = -1475419;
        QUAL2ERROR_DIV3_TABLE[67] = -1475419;
        QUAL2ERROR_DIV3_TABLE[68] = -1475419;
        QUAL2ERROR_DIV3_TABLE[69] = -1475419;
        QUAL2ERROR_DIV3_TABLE[70] = -1475419;
        QUAL2ERROR_DIV3_TABLE[71] = -1475419;
        QUAL2ERROR_DIV3_TABLE[72] = -1475419;
        QUAL2ERROR_DIV3_TABLE[73] = -1475419;
        QUAL2ERROR_DIV3_TABLE[74] = -1475419;
        QUAL2ERROR_DIV3_TABLE[75] = -1475419;
        QUAL2ERROR_DIV3_TABLE[76] = -1475419;
        QUAL2ERROR_DIV3_TABLE[77] = -1475419;
        QUAL2ERROR_DIV3_TABLE[78] = -1475419;
        QUAL2ERROR_DIV3_TABLE[79] = -1475419;
        QUAL2ERROR_DIV3_TABLE[80] = -1475419;
        QUAL2ERROR_DIV3_TABLE[81] = -1475419;
        QUAL2ERROR_DIV3_TABLE[82] = -1475419;
        QUAL2ERROR_DIV3_TABLE[83] = -1475419;
        QUAL2ERROR_DIV3_TABLE[84] = -1475419;
        QUAL2ERROR_DIV3_TABLE[85] = -1475419;
        QUAL2ERROR_DIV3_TABLE[86] = -1475419;
        QUAL2ERROR_DIV3_TABLE[87] = -1475419;
        QUAL2ERROR_DIV3_TABLE[88] = -1475419;
        QUAL2ERROR_DIV3_TABLE[89] = -1475419;
        QUAL2ERROR_DIV3_TABLE[90] = -1475419;
        QUAL2ERROR_DIV3_TABLE[91] = -1475419;
        QUAL2ERROR_DIV3_TABLE[92] = -1475419;
        QUAL2ERROR_DIV3_TABLE[93] = -1475419;
        QUAL2ERROR_DIV3_TABLE[94] = -1475419;
        QUAL2ERROR_DIV3_TABLE[95] = -1475419;
        QUAL2ERROR_DIV3_TABLE[96] = -1475419;
        QUAL2ERROR_DIV3_TABLE[97] = -1475419;
        QUAL2ERROR_DIV3_TABLE[98] = -1475419;
        QUAL2ERROR_DIV3_TABLE[99] = -1475419;
        QUAL2ERROR_DIV3_TABLE[100] = -1475419;
        QUAL2ERROR_DIV3_TABLE[101] = -1475419;
        QUAL2ERROR_DIV3_TABLE[102] = -1475419;
        QUAL2ERROR_DIV3_TABLE[103] = -1475419;
        QUAL2ERROR_DIV3_TABLE[104] = -1475419;
        QUAL2ERROR_DIV3_TABLE[105] = -1475419;
        QUAL2ERROR_DIV3_TABLE[106] = -1475419;
        QUAL2ERROR_DIV3_TABLE[107] = -1475419;
        QUAL2ERROR_DIV3_TABLE[108] = -1475419;
        QUAL2ERROR_DIV3_TABLE[109] = -1475419;
        QUAL2ERROR_DIV3_TABLE[110] = -1475419;
        QUAL2ERROR_DIV3_TABLE[111] = -1475419;
        QUAL2ERROR_DIV3_TABLE[112] = -1475419;
        QUAL2ERROR_DIV3_TABLE[113] = -1475419;
        QUAL2ERROR_DIV3_TABLE[114] = -1475419;
        QUAL2ERROR_DIV3_TABLE[115] = -1475419;
        QUAL2ERROR_DIV3_TABLE[116] = -1475419;
        QUAL2ERROR_DIV3_TABLE[117] = -1475419;
        QUAL2ERROR_DIV3_TABLE[118] = -1475419;
        QUAL2ERROR_DIV3_TABLE[119] = -1475419;
        QUAL2ERROR_DIV3_TABLE[120] = -1475419;
        QUAL2ERROR_DIV3_TABLE[121] = -1475419;
        QUAL2ERROR_DIV3_TABLE[122] = -1475419;
        QUAL2ERROR_DIV3_TABLE[123] = -1475419;
        QUAL2ERROR_DIV3_TABLE[124] = -1475419;
        QUAL2ERROR_DIV3_TABLE[125] = -1475419;
        QUAL2ERROR_DIV3_TABLE[126] = -1475419;
        QUAL2ERROR_DIV3_TABLE[127] = -1475419;
        QUAL2ERROR_DIV3_TABLE[128] = -1475419;
        QUAL2ERROR_DIV3_TABLE[129] = -1475419;
        QUAL2ERROR_DIV3_TABLE[130] = -1475419;
        QUAL2ERROR_DIV3_TABLE[131] = -1475419;
        QUAL2ERROR_DIV3_TABLE[132] = -1475419;
        QUAL2ERROR_DIV3_TABLE[133] = -1475419;
        QUAL2ERROR_DIV3_TABLE[134] = -1475419;
        QUAL2ERROR_DIV3_TABLE[135] = -1475419;
        QUAL2ERROR_DIV3_TABLE[136] = -1475419;
        QUAL2ERROR_DIV3_TABLE[137] = -1475419;
        QUAL2ERROR_DIV3_TABLE[138] = -1475419;
        QUAL2ERROR_DIV3_TABLE[139] = -1475419;
        QUAL2ERROR_DIV3_TABLE[140] = -1475419;
        QUAL2ERROR_DIV3_TABLE[141] = -1475419;
        QUAL2ERROR_DIV3_TABLE[142] = -1475419;
        QUAL2ERROR_DIV3_TABLE[143] = -1475419;
        QUAL2ERROR_DIV3_TABLE[144] = -1475419;
        QUAL2ERROR_DIV3_TABLE[145] = -1475419;
        QUAL2ERROR_DIV3_TABLE[146] = -1475419;
        QUAL2ERROR_DIV3_TABLE[147] = -1475419;
        QUAL2ERROR_DIV3_TABLE[148] = -1475419;
        QUAL2ERROR_DIV3_TABLE[149] = -1475419;
        QUAL2ERROR_DIV3_TABLE[150] = -1475419;
        QUAL2ERROR_DIV3_TABLE[151] = -1475419;
        QUAL2ERROR_DIV3_TABLE[152] = -1475419;
        QUAL2ERROR_DIV3_TABLE[153] = -1475419;
        QUAL2ERROR_DIV3_TABLE[154] = -1475419;
        QUAL2ERROR_DIV3_TABLE[155] = -1475419;
        QUAL2ERROR_DIV3_TABLE[156] = -1475419;
        QUAL2ERROR_DIV3_TABLE[157] = -1475419;
        QUAL2ERROR_DIV3_TABLE[158] = -1475419;
        QUAL2ERROR_DIV3_TABLE[159] = -1475419;
        QUAL2ERROR_DIV3_TABLE[160] = -1475419;
        QUAL2ERROR_DIV3_TABLE[161] = -1475419;
        QUAL2ERROR_DIV3_TABLE[162] = -1475419;
        QUAL2ERROR_DIV3_TABLE[163] = -1475419;
        QUAL2ERROR_DIV3_TABLE[164] = -1475419;
        QUAL2ERROR_DIV3_TABLE[165] = -1475419;
        QUAL2ERROR_DIV3_TABLE[166] = -1475419;
        QUAL2ERROR_DIV3_TABLE[167] = -1475419;
        QUAL2ERROR_DIV3_TABLE[168] = -1475419;
        QUAL2ERROR_DIV3_TABLE[169] = -1475419;
        QUAL2ERROR_DIV3_TABLE[170] = -1475419;
        QUAL2ERROR_DIV3_TABLE[171] = -1475419;
        QUAL2ERROR_DIV3_TABLE[172] = -1475419;
        QUAL2ERROR_DIV3_TABLE[173] = -1475419;
        QUAL2ERROR_DIV3_TABLE[174] = -1475419;
        QUAL2ERROR_DIV3_TABLE[175] = -1475419;
        QUAL2ERROR_DIV3_TABLE[176] = -1475419;
        QUAL2ERROR_DIV3_TABLE[177] = -1475419;
        QUAL2ERROR_DIV3_TABLE[178] = -1475419;
        QUAL2ERROR_DIV3_TABLE[179] = -1475419;
        QUAL2ERROR_DIV3_TABLE[180] = -1475419;
        QUAL2ERROR_DIV3_TABLE[181] = -1475419;
        QUAL2ERROR_DIV3_TABLE[182] = -1475419;
        QUAL2ERROR_DIV3_TABLE[183] = -1475419;
        QUAL2ERROR_DIV3_TABLE[184] = -1475419;
        QUAL2ERROR_DIV3_TABLE[185] = -1475419;
        QUAL2ERROR_DIV3_TABLE[186] = -1475419;
        QUAL2ERROR_DIV3_TABLE[187] = -1475419;
        QUAL2ERROR_DIV3_TABLE[188] = -1475419;
        QUAL2ERROR_DIV3_TABLE[189] = -1475419;
        QUAL2ERROR_DIV3_TABLE[190] = -1475419;
        QUAL2ERROR_DIV3_TABLE[191] = -1475419;
        QUAL2ERROR_DIV3_TABLE[192] = -1475419;
        QUAL2ERROR_DIV3_TABLE[193] = -1475419;
        QUAL2ERROR_DIV3_TABLE[194] = -1475419;
        QUAL2ERROR_DIV3_TABLE[195] = -1475419;
        QUAL2ERROR_DIV3_TABLE[196] = -1475419;
        QUAL2ERROR_DIV3_TABLE[197] = -1475419;
        QUAL2ERROR_DIV3_TABLE[198] = -1475419;
        QUAL2ERROR_DIV3_TABLE[199] = -1475419;
        QUAL2ERROR_DIV3_TABLE[200] = -1475419;
        QUAL2ERROR_DIV3_TABLE[201] = -1475419;
        QUAL2ERROR_DIV3_TABLE[202] = -1475419;
        QUAL2ERROR_DIV3_TABLE[203] = -1475419;
        QUAL2ERROR_DIV3_TABLE[204] = -1475419;
        QUAL2ERROR_DIV3_TABLE[205] = -1475419;
        QUAL2ERROR_DIV3_TABLE[206] = -1475419;
        QUAL2ERROR_DIV3_TABLE[207] = -1475419;
        QUAL2ERROR_DIV3_TABLE[208] = -1475419;
        QUAL2ERROR_DIV3_TABLE[209] = -1475419;
        QUAL2ERROR_DIV3_TABLE[210] = -1475419;
        QUAL2ERROR_DIV3_TABLE[211] = -1475419;
        QUAL2ERROR_DIV3_TABLE[212] = -1475419;
        QUAL2ERROR_DIV3_TABLE[213] = -1475419;
        QUAL2ERROR_DIV3_TABLE[214] = -1475419;
        QUAL2ERROR_DIV3_TABLE[215] = -1475419;
        QUAL2ERROR_DIV3_TABLE[216] = -1475419;
        QUAL2ERROR_DIV3_TABLE[217] = -1475419;
        QUAL2ERROR_DIV3_TABLE[218] = -1475419;
        QUAL2ERROR_DIV3_TABLE[219] = -1475419;
        QUAL2ERROR_DIV3_TABLE[220] = -1475419;
        QUAL2ERROR_DIV3_TABLE[221] = -1475419;
        QUAL2ERROR_DIV3_TABLE[222] = -1475419;
        QUAL2ERROR_DIV3_TABLE[223] = -1475419;
        QUAL2ERROR_DIV3_TABLE[224] = -1475419;
        QUAL2ERROR_DIV3_TABLE[225] = -1475419;
        QUAL2ERROR_DIV3_TABLE[226] = -1475419;
        QUAL2ERROR_DIV3_TABLE[227] = -1475419;
        QUAL2ERROR_DIV3_TABLE[228] = -1475419;
        QUAL2ERROR_DIV3_TABLE[229] = -1475419;
        QUAL2ERROR_DIV3_TABLE[230] = -1475419;
        QUAL2ERROR_DIV3_TABLE[231] = -1475419;
        QUAL2ERROR_DIV3_TABLE[232] = -1475419;
        QUAL2ERROR_DIV3_TABLE[233] = -1475419;
        QUAL2ERROR_DIV3_TABLE[234] = -1475419;
        QUAL2ERROR_DIV3_TABLE[235] = -1475419;
        QUAL2ERROR_DIV3_TABLE[236] = -1475419;
        QUAL2ERROR_DIV3_TABLE[237] = -1475419;
        QUAL2ERROR_DIV3_TABLE[238] = -1475419;
        QUAL2ERROR_DIV3_TABLE[239] = -1475419;
        QUAL2ERROR_DIV3_TABLE[240] = -1475419;
        QUAL2ERROR_DIV3_TABLE[241] = -1475419;
        QUAL2ERROR_DIV3_TABLE[242] = -1475419;
        QUAL2ERROR_DIV3_TABLE[243] = -1475419;
        QUAL2ERROR_DIV3_TABLE[244] = -1475419;
        QUAL2ERROR_DIV3_TABLE[245] = -1475419;
        QUAL2ERROR_DIV3_TABLE[246] = -1475419;
        QUAL2ERROR_DIV3_TABLE[247] = -1475419;
        QUAL2ERROR_DIV3_TABLE[248] = -1475419;
        QUAL2ERROR_DIV3_TABLE[249] = -1475419;
        QUAL2ERROR_DIV3_TABLE[250] = -1475419;
        QUAL2ERROR_DIV3_TABLE[251] = -1475419;
        QUAL2ERROR_DIV3_TABLE[252] = -1475419;
        QUAL2ERROR_DIV3_TABLE[253] = -1475419;
        QUAL2ERROR_DIV3_TABLE[254] = -1475419;
        QUAL2ERROR_DIV3_TABLE[255] = -1475419;

}

alu_32::alu_32() {
    
    logSumTable_init();

}

alu_32::~alu_32() {}

int alu_32::execute_8bit(int input_0, int input_1, int op) {

	// int8_t *input_0_simd, *input_1_simd, *output_simd;
	// input_0_simd = (int8_t*) malloc (SIMD_WIDTH8 * sizeof(int8_t));
	// input_1_simd = (int8_t*) malloc (SIMD_WIDTH8 * sizeof(int8_t));
	// output_simd = (int8_t*) malloc (SIMD_WIDTH8 * sizeof(int8_t));
	// int8_t input_0_simd[SIMD_WIDTH8], input_1_simd[SIMD_WIDTH8], output_simd[SIMD_WIDTH8];
	int zero = 0;
	memcpy(output_simd, &zero, SIMD_WIDTH8 * sizeof(int8_t));
	memcpy(input_0_simd, &input_0, SIMD_WIDTH8 * sizeof(int8_t));
	memcpy(input_1_simd, &input_1, SIMD_WIDTH8 * sizeof(int8_t));
	int i, output;

	switch(op) {
		case ADDITION: {
			for (i=0; i<SIMD_WIDTH8; i++)
				output_simd[i] = input_0_simd[i] + input_1_simd[i];
			break;
		}
		case SUBTRACTION: {
			for (i=0; i<SIMD_WIDTH8; i++)
				output_simd[i] = input_0_simd[i] - input_1_simd[i];
			break;
		}
		case MAXIMUM: {
			for (i=0; i<SIMD_WIDTH8; i++)
				output_simd[i] = (input_0_simd[i] - input_1_simd[i]) >= 0 ? input_0_simd[i] : input_1_simd[i];
			break;
		}
		case MINIMUM: {
			for (i=0; i<SIMD_WIDTH8; i++)
				output_simd[i] = (input_0_simd[i] - input_1_simd[i]) >= 0 ? input_1_simd[i] : input_0_simd[i];
			break;
		}
		case COPY: {
			for (i=0; i<SIMD_WIDTH8; i++)
				output_simd[i] = input_0_simd[i];
			break;
		}
		case INVALID: {
			for (i=0; i<SIMD_WIDTH8; i++)
				output_simd[i] = 0;
			break;
		}
	}
	memcpy(&output, output_simd, SIMD_WIDTH8 * sizeof(int8_t));
	return output;
}

int alu_32::execute(int input_0, int input_1, int op) {
	int out, log_sum_in, tmp_shift, tmp_minus;
	long tmp, in_0_tmp, in_1_tmp, sum; 
	if (input_0 < 0) in_0_tmp = pow(2, 32) + input_0;
	else in_0_tmp = input_0;
	if (input_1 < 0) in_1_tmp = pow(2, 32) + input_1;
	else in_1_tmp = input_1;
	
	switch(op){
		case ADDITION: {
			out = input_0 + input_1;
			break;
		}
		case SUBTRACTION: {
			out = input_0 - input_1;
			break;
		}
		case CARRY: {
			sum = in_0_tmp + in_1_tmp;
			if (sum >= pow(2, 32)) out = 1;
			else out = 0;
			break;
		}
		case BORROW: {
			// out = (input_0 >= input_1) ? 0 : 1;
			out = (input_0 - input_1) >= 0 ? 0 : 1;
			break;
		}
		case MAXIMUM: {
			// out = std::max(input_0, input_1);
			out = (input_0 - input_1) >= 0 ? input_0 : input_1;
			break;
		}
		case MINIMUM: {
			// out = std::min(input_0, input_1);
			out = (input_0 - input_1) >= 0 ? input_1 : input_0;
			break;
		}
		case LEFT_SHIFT: {
			if (input_0 < 0) tmp = pow(2, 32) + input_0;
			else tmp = input_0;
			out = tmp << 16;
			break;
		}
		case RIGHT_SHIFT: {
			if (input_0 < 0) tmp = pow(2, 32) + input_0;
			else tmp = input_0;
			out = tmp >> 16;
			break;
		}
		case COPY: {
			out = input_0;
			break;
		}
		case LOG2_LUT: {
			out = ilog2_32(input_0) >> 1;
			break;
		}
		case LOG_SUM_LUT: {
			tmp_minus = -input_0;
			tmp_shift = input_0 < 0 ? (tmp_minus >> 16) : (input_0 >> 16);
			log_sum_in = tmp_shift > 16 ? 16 : tmp_shift;

			if (log_sum_in % 32 < 0) tmp = 32 + log_sum_in % 32;
			else tmp = log_sum_in % 32;
			if (tmp >= 16) out = logSumTable[16];
			else out = logSumTable[tmp];
			break;
		}
        case INVALID: {
			out = 0;
			break;
		}
		default: {
			out = 0;
			break;
		}
	}
	return out;
}


int alu_32::execute_4input_8bit(int input_0, int input_1, int input_2, int input_3, int op) {

	// int8_t *input_0_simd, *input_1_simd, *input_2_simd, *input_3_simd, *output_simd;
	// input_0_simd = (int8_t*) malloc (SIMD_WIDTH8 * sizeof(int8_t));
	// input_1_simd = (int8_t*) malloc (SIMD_WIDTH8 * sizeof(int8_t));
	// input_2_simd = (int8_t*) malloc (SIMD_WIDTH8 * sizeof(int8_t));
	// input_3_simd = (int8_t*) malloc (SIMD_WIDTH8 * sizeof(int8_t));
	// output_simd = (int8_t*) malloc (SIMD_WIDTH8 * sizeof(int8_t));
	// int8_t input_0_simd[SIMD_WIDTH8], input_1_simd[SIMD_WIDTH8], input_3_simd[SIMD_WIDTH8], input_2_simd[SIMD_WIDTH8], output_simd[SIMD_WIDTH8];
	int zero = 0;
	memcpy(output_simd, &zero, SIMD_WIDTH8 * sizeof(int8_t));
	memcpy(input_0_simd, &input_0, SIMD_WIDTH8 * sizeof(int8_t));
	memcpy(input_1_simd, &input_1, SIMD_WIDTH8 * sizeof(int8_t));
	memcpy(input_2_simd, &input_2, SIMD_WIDTH8 * sizeof(int8_t));
	memcpy(input_3_simd, &input_3, SIMD_WIDTH8 * sizeof(int8_t));
	int i, output;

	switch(op) {
		case ADDITION: {
			for (i=0; i<SIMD_WIDTH8; i++)
				output_simd[i] = input_0_simd[i] + input_1_simd[i];
			break;
		}
		case SUBTRACTION: {
			for (i=0; i<SIMD_WIDTH8; i++)
				output_simd[i] = input_0_simd[i] - input_1_simd[i];
			break;
		}
		case MAXIMUM: {
			for (i=0; i<SIMD_WIDTH8; i++)
				output_simd[i] = (input_0_simd[i] - input_1_simd[i]) >= 0 ? input_0_simd[i] : input_1_simd[i];
			break;
		}
		case MINIMUM: {
			for (i=0; i<SIMD_WIDTH8; i++)
				output_simd[i] = (input_0_simd[i] - input_1_simd[i]) >= 0 ? input_1_simd[i] : input_0_simd[i];
			break;
		}
		case COPY: {
			for (i=0; i<SIMD_WIDTH8; i++)
				output_simd[i] = input_0_simd[i];
			break;
		}
		case COMP_LARGER: {
			for (i=0; i<SIMD_WIDTH8; i++)
				output_simd[i] = (input_0_simd[i] > input_1_simd[i]) ? input_2_simd[i] : input_3_simd[i];
			break;
		}
		case COMP_EQUAL: {
			for (i=0; i<SIMD_WIDTH8; i++)
				output_simd[i] = (input_0_simd[i] == input_1_simd[i]) ? input_2_simd[i] : input_3_simd[i];
			break;
		}
		case INVALID: {
			for (i=0; i<SIMD_WIDTH8; i++)
				output_simd[i] = 0;
			break;
		}
	}
	memcpy(&output, output_simd, SIMD_WIDTH8 * sizeof(int8_t));
	return output;
}


int alu_32::execute_4input(int input_0, int input_1, int input_2, int input_3, int op) {
	int out, tmp_minus, tmp_shift, log_sum_in;
	long tmp, in_0_tmp, in_1_tmp, sum; 
	if (input_0 < 0) in_0_tmp = pow(2, 32) + input_0;
	else in_0_tmp = input_0;
	if (input_1 < 0) in_1_tmp = pow(2, 32) + input_1;
	else in_1_tmp = input_1;

	switch(op){
		case ADDITION: {
			out = input_0 + input_1;
			break;
		}
		case SUBTRACTION: {
			out = input_0 - input_1;
			break;
		}
		case CARRY: {
			sum = in_0_tmp + in_1_tmp;
			if (sum >= pow(2, 32)) out = 1;
			else out = 0;
			break;
		}
		case BORROW: {
			out = (input_0 > input_1) ? 0 : 1;
			break;
		}
		case MAXIMUM: {
			out = (input_0 - input_1) > 0 ? input_0 : input_1;
			break;
		}
		case MINIMUM: {
			out = (input_0 - input_1) > 0 ? input_1 : input_0;
			break;
		}
		case LEFT_SHIFT: {
			if (input_0 < 0) tmp = pow(2, 32) + input_0;
			else tmp = input_0;
			out = tmp << 16;
			break;
		}
		case RIGHT_SHIFT: {
			if (input_0 < 0) tmp = pow(2, 32) + input_0;
			else tmp = input_0;
			out = tmp >> 16;
			break;
		}
		case COPY: {
			out = input_0;
			break;
		}
		case LOG2_LUT: {
			out = ilog2_32(input_0) >> 1;
			break;
		}
		case LOG_SUM_LUT: {
			tmp_minus = -input_0;
			tmp_shift = input_0 < 0 ? (tmp_minus >> 16) : (input_0 >> 16);
			log_sum_in = tmp_shift > 16 ? 16 : tmp_shift;

			if (log_sum_in % 32 < 0) tmp = 32 + log_sum_in % 32;
			else tmp = log_sum_in % 32;
			if (tmp >= 16) out = logSumTable[16];
			else out = logSumTable[tmp];
			break;
		}
		case COMP_LARGER: {
			out = (input_0 > input_1) ? input_2 : input_3;
			break;
		}
		case COMP_EQUAL: {
			out = (input_0 == input_1) ? input_2 : input_3;
			break;
		}
		case INVALID: {
			out = 0;
			break;
		}
		default: {
			out = 0;
			break;
		}
	}
	return out;
}

void alu_32::logSumTable_init() {
    logSumTable[0] = 0b00000000000000010000000000000000;
    logSumTable[1] = 0b00000000000000001001010111000001;
    logSumTable[2] = 0b00000000000000000101001001101010;
    logSumTable[3] = 0b00000000000000000010101110000001;
    logSumTable[4] = 0b00000000000000000001011001100100;
    logSumTable[5] = 0b00000000000000000000101101011110;
    logSumTable[6] = 0b00000000000000000000010110111010;
    logSumTable[7] = 0b00000000000000000000001011100000;
    logSumTable[8] = 0b00000000000000000000000101110001;
    logSumTable[9] = 0b00000000000000000000000010111001;
    logSumTable[10] = 0b00000000000000000000000001011101;
    logSumTable[11] = 0b00000000000000000000000000101111;
    logSumTable[12] = 0b00000000000000000000000000011000;
    logSumTable[13] = 0b00000000000000000000000000001100;
    logSumTable[14] = 0b00000000000000000000000000000110;
    logSumTable[15] = 0b00000000000000000000000000000011;
    logSumTable[16] = 0b00000000000000000000000000000010;
}

int alu_32::ilog2_32(unsigned int v) {
    unsigned int t, tt, log2;
    if ((tt = v>>16)) log2 = (t = tt>>8) ? 24 + LogTable256[t] : 16 + LogTable256[tt];
    log2 = (t = v>>8) ? 8 + LogTable256[t] : LogTable256[v];
	return log2;
}

comp_decoder::comp_decoder() {}
comp_decoder::~comp_decoder() {}

void comp_decoder::execute(long instruction, int* op, int* in_addr, int* out_addr, int* PC) {

    // instruction: op[0] op[1] op[2] in_addr[0] in_addr[1] in_addr[2] in_addr[3] in_addr[4] in_addr[5] out_addr

    int i;
    unsigned long out_addr_mask, in_addr_mask[6], op_mask[3];

    out_addr_mask = (1 << REGFILE_ADDR_WIDTH) - 1;
    *out_addr = (int)(out_addr_mask & instruction);

    for (i = 0; i < 6; i++) {
        in_addr_mask[i] = (unsigned long)((1 << REGFILE_ADDR_WIDTH) - 1) << (6 - i) * REGFILE_ADDR_WIDTH;
        // printf("%d %lx %lx\n", i, in_addr_mask[i], instruction);
        in_addr[i] = (in_addr_mask[i] & instruction) >> (6 - i) * REGFILE_ADDR_WIDTH;
    }

    for (i = 0; i < 3; i++) {
        op_mask[i] = (unsigned long)((1 << COMP_OPCODE_WIDTH) - 1) << (7 * REGFILE_ADDR_WIDTH + (2 - i) * COMP_OPCODE_WIDTH);
        op[i] = (op_mask[i] & instruction) >> (7 * REGFILE_ADDR_WIDTH + (2 - i) * COMP_OPCODE_WIDTH);
    }

    if (op[0] < HALT) (*PC)++;
    else if (op[0] == HALT) (*PC) = (*PC);
    else {
        fprintf(stderr, "compute opcode %d error.\n", op[0]);
        exit(-1);
    }

    // printf("%d %d %d %d %d %d %d %d %d %d\t", op[0], op[1], op[2], in_addr[0], in_addr[1], in_addr[2], in_addr[3], in_addr[4], in_addr[5], *out_addr);
#ifdef PROFILE
    printf("%d %d %d %d %d %d %d\t", in_addr[0], in_addr[1], in_addr[2], in_addr[3], in_addr[4], in_addr[5], *out_addr);
#endif
}