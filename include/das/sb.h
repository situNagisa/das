#include <windows.h>
#include <formatio.h>
#include <ansi_c.h>
#include <analysis.h>
#include <utility.h>
#include <cvirte.h>		
#include <userint.h>
#include "PCIe6920_DEMO.h"
#include "pcie6920/pcie6920.h"

using namespace ::pcie6920::c_api;

static int panelHandle;
int status;

int CVICALLBACK ReadDataThread(void* functionData);
int ReadDataId;
int read_data_enable = 0;

unsigned int recv_enable_cmd = 0;
unsigned int frame_num;
unsigned int space_index_point;

unsigned int total_point_num;

unsigned int data_src_sel;
unsigned int sw_dem;
unsigned int upload_rate_sel;
unsigned int dem_ch_quantity;

unsigned int upload_ch_quantity;

unsigned int space_time_sel;

short* p_s_buf;
short* p_ch0_buf, * p_ch1_buf, * p_ch2_buf, * p_ch3_buf;
short* p_ch0_space_data_buf, * p_ch1_space_data_buf;

double* p_arctan, * p_power_sqrt;

unsigned int spectrum_en, psd_en;
unsigned int spectrum_ch_sel;

unsigned int save_data_en;
int file_handle;
char fileName[MAX_PATHNAME_LEN];
unsigned int file_no = 0;
void start2create_data_file();
void spectrum_analyze(short* p_data_in, unsigned int point_num, unsigned int psd_sel, double sample_rate, double* p_spectrum, double* p_df);
double* p_spectrum;

int main(int argc, char* argv[])
{
	if (InitCVIRTE(0, argv, 0) == 0)
		return -1;	/* out of memory */
	if ((panelHandle = LoadPanel(0, "PCIe6920_DEMO.uir", PANEL)) < 0)
		return -1;

	if ((status = pcie6920_open()) != 0) {
		MessagePopup("Warning", "Open Device Failed!");
		return -1;
	}

	else {
		SetCtrlVal(panelHandle, PANEL_LED, 1);
	}

	CmtScheduleThreadPoolFunctionAdv(DEFAULT_THREAD_POOL_HANDLE, ReadDataThread, NULL, THREAD_PRIORITY_NORMAL, 0,
		EVENT_TP_THREAD_FUNCTION_END, 0, CmtGetCurrentThreadID(), &ReadDataId);

	DisplayPanel(panelHandle);
	RunUserInterface();
	DiscardPanel(panelHandle);
	return 0;
}

int CVICALLBACK QuitCallback(int panel, int control, int event,
	void* callbackData, int eventData1, int eventData2)
{
	switch (event)
	{
	case EVENT_COMMIT:
		pcie6920_close();
		QuitUserInterface(0);
		break;
	}
	return 0;
}


int CVICALLBACK ReadDataThread(void* functionData)
{

	int i;
	unsigned int point_num_in_buf_per_ch;;
	unsigned int points_per_ch_returned;
	double df;

	unsigned int display_ch_index = 0;

	while (1) {

		if (read_data_enable) {
			recv_enable_cmd = 1;

			if (upload_ch_quantity == 4) GetCtrlVal(panelHandle, PANEL_BINARYSWITCH_8, &display_ch_index);
			//read raw data
			if (!save_data_en) {  //not save file, display data
				do {
					Delay(0.001);
					pcie6920_point_num_per_ch_in_buf_query(&point_num_in_buf_per_ch);
				} while (point_num_in_buf_per_ch < (total_point_num * frame_num));

				pcie6920_read_data(total_point_num * frame_num, p_s_buf, &points_per_ch_returned);

				pcie6920_point_num_per_ch_in_buf_query(&point_num_in_buf_per_ch);
				SetCtrlVal(panelHandle, PANEL_NUMERIC_12, point_num_in_buf_per_ch * upload_ch_quantity / 1024 / 1024);

				DeleteGraphPlot(panelHandle, PANEL_GRAPH, -1, VAL_IMMEDIATE_DRAW);
				DeleteGraphPlot(panelHandle, PANEL_GRAPH_2, -1, VAL_IMMEDIATE_DRAW);

				if (space_time_sel) { //space mode
					/*
					for(i=0;i<total_point_num*frame_num;i++) {
						p_ch0_buf[i]=p_s_buf[2*i];
						p_ch1_buf[i]=p_s_buf[2*i+1];
					}

					GetCtrlVal(panelHandle,PANEL_NUMERIC_17,&space_index_point);
					for(i=0;i<frame_num;i++) {
						p_ch0_space_data_buf[i]=p_ch0_buf[space_index_point+total_point_num*i];
						p_ch1_space_data_buf[i]=p_ch1_buf[space_index_point+total_point_num*i];
					}

					PlotY (panelHandle, PANEL_GRAPH, p_ch0_space_data_buf, frame_num, VAL_SHORT_INTEGER, VAL_THIN_LINE, VAL_EMPTY_SQUARE, VAL_SOLID, 1, VAL_YELLOW);
					PlotY (panelHandle, PANEL_GRAPH_2, p_ch1_space_data_buf, frame_num, VAL_SHORT_INTEGER, VAL_THIN_LINE, VAL_EMPTY_SQUARE, VAL_SOLID, 1, VAL_WHITE);
					*/
				}
				else { //time mode
					if (spectrum_en) {
						for (i = 0; i < total_point_num; i++) {
							p_ch0_buf[i] = p_s_buf[upload_ch_quantity * i];
							p_ch1_buf[i] = p_s_buf[upload_ch_quantity * i + 1];
						}
						if (spectrum_ch_sel == 0) {
							PlotY(panelHandle, PANEL_GRAPH, p_ch0_buf, total_point_num, VAL_SHORT_INTEGER, VAL_THIN_LINE, VAL_EMPTY_SQUARE, VAL_SOLID, 1, VAL_GREEN);
							spectrum_analyze(p_ch0_buf, total_point_num, psd_en, 250000000.0 / upload_rate_sel, p_spectrum, &df);
							//PlotY (panelHandle, PANEL_GRAPH_2, p_spectrum, total_point_num, VAL_DOUBLE, VAL_THIN_LINE, VAL_EMPTY_SQUARE, VAL_SOLID, 1, VAL_YELLOW);
							PlotWaveform(panelHandle, PANEL_GRAPH_2, p_spectrum, total_point_num, VAL_DOUBLE, 1.0, 0.0, 0.0, df, VAL_THIN_LINE, VAL_EMPTY_SQUARE, VAL_SOLID, 1, VAL_YELLOW);
						}
						else {
							PlotY(panelHandle, PANEL_GRAPH, p_ch1_buf, total_point_num, VAL_SHORT_INTEGER, VAL_THIN_LINE, VAL_EMPTY_SQUARE, VAL_SOLID, 1, VAL_GREEN);
							spectrum_analyze(p_ch1_buf, total_point_num, psd_en, 250000000.0 / upload_rate_sel, p_spectrum, &df);
							//PlotY (panelHandle, PANEL_GRAPH_2, p_spectrum, total_point_num, VAL_DOUBLE, VAL_THIN_LINE, VAL_EMPTY_SQUARE, VAL_SOLID, 1, VAL_YELLOW);
							PlotWaveform(panelHandle, PANEL_GRAPH_2, p_spectrum, total_point_num, VAL_DOUBLE, 1.0, 0.0, 0.0, df, VAL_THIN_LINE, VAL_EMPTY_SQUARE, VAL_SOLID, 1, VAL_YELLOW);
						}
					}
					else {  //display data only
						for (i = 0; i < total_point_num * 4; i++) {
							p_ch0_buf[i] = p_s_buf[upload_ch_quantity * i + 0];
							p_ch1_buf[i] = p_s_buf[upload_ch_quantity * i + 1];
							if (upload_ch_quantity == 4) {
								p_ch2_buf[i] = p_s_buf[upload_ch_quantity * i + 2];
								p_ch3_buf[i] = p_s_buf[upload_ch_quantity * i + 3];
							}
						}
						if (sw_dem) {
							for (i = 0; i < total_point_num; i++) {
								if (display_ch_index == 0) 		p_arctan[i] = atan2((p_ch1_buf[i] * 1.0), (p_ch0_buf[i] * 1.0));
								else if (display_ch_index == 1) 	p_arctan[i] = atan2((p_ch3_buf[i] * 1.0), (p_ch2_buf[i] * 1.0));
							}
							PlotY(panelHandle, PANEL_GRAPH, p_arctan, total_point_num, VAL_DOUBLE, VAL_THIN_LINE, VAL_EMPTY_SQUARE, VAL_SOLID, 1, VAL_WHITE);

							//dispaly power sqrt 4 frame
							//0-frame
							for (i = 0; i < total_point_num; i++) {
								if (display_ch_index == 0) 		p_power_sqrt[i] = sqrt(p_ch0_buf[i] * p_ch0_buf[i] / 2.0 + p_ch1_buf[i] * p_ch1_buf[i] / 2.0);
								else if (display_ch_index == 1) 	p_power_sqrt[i] = sqrt(p_ch2_buf[i] * p_ch2_buf[i] / 2.0 + p_ch3_buf[i] * p_ch3_buf[i] / 2.0);
							}
							PlotY(panelHandle, PANEL_GRAPH_2, p_power_sqrt, total_point_num, VAL_DOUBLE, VAL_THIN_LINE, VAL_EMPTY_SQUARE, VAL_SOLID, 1, VAL_YELLOW);
							//1-frame
							for (i = 0; i < total_point_num; i++) {
								if (display_ch_index == 0) 		p_power_sqrt[i] = sqrt(p_ch0_buf[total_point_num + i] * p_ch0_buf[total_point_num + i] / 2.0 + p_ch1_buf[total_point_num + i] * p_ch1_buf[total_point_num + i] / 2.0);
								else if (display_ch_index == 1) 	p_power_sqrt[i] = sqrt(p_ch2_buf[total_point_num + i] * p_ch2_buf[total_point_num + i] / 2.0 + p_ch3_buf[total_point_num + i] * p_ch3_buf[total_point_num + i] / 2.0);
							}
							PlotY(panelHandle, PANEL_GRAPH_2, p_power_sqrt, total_point_num, VAL_DOUBLE, VAL_THIN_LINE, VAL_EMPTY_SQUARE, VAL_SOLID, 1, VAL_RED);
							//2-frame
							for (i = 0; i < total_point_num; i++) {
								if (display_ch_index == 0) 		p_power_sqrt[i] = sqrt(p_ch0_buf[2 * total_point_num + i] * p_ch0_buf[2 * total_point_num + i] / 2.0 + p_ch1_buf[2 * total_point_num + i] * p_ch1_buf[2 * total_point_num + i] / 2.0);
								else if (display_ch_index == 1) 	p_power_sqrt[i] = sqrt(p_ch2_buf[2 * total_point_num + i] * p_ch2_buf[2 * total_point_num + i] / 2.0 + p_ch3_buf[2 * total_point_num + i] * p_ch3_buf[2 * total_point_num + i] / 2.0);
							}
							PlotY(panelHandle, PANEL_GRAPH_2, p_power_sqrt, total_point_num, VAL_DOUBLE, VAL_THIN_LINE, VAL_EMPTY_SQUARE, VAL_SOLID, 1, VAL_BLUE);
							//3-frame
							for (i = 0; i < total_point_num; i++) {
								if (display_ch_index == 0) 		p_power_sqrt[i] = sqrt(p_ch0_buf[3 * total_point_num + i] * p_ch0_buf[3 * total_point_num + i] / 2.0 + p_ch1_buf[3 * total_point_num + i] * p_ch1_buf[3 * total_point_num + i] / 2.0);
								else if (display_ch_index == 1) 	p_power_sqrt[i] = sqrt(p_ch2_buf[3 * total_point_num + i] * p_ch2_buf[3 * total_point_num + i] / 2.0 + p_ch3_buf[3 * total_point_num + i] * p_ch3_buf[3 * total_point_num + i] / 2.0);
							}
							PlotY(panelHandle, PANEL_GRAPH_2, p_power_sqrt, total_point_num, VAL_DOUBLE, VAL_THIN_LINE, VAL_EMPTY_SQUARE, VAL_SOLID, 1, VAL_GREEN);

						}

						else if (data_src_sel == 0) { //raw data
							PlotY(panelHandle, PANEL_GRAPH, p_ch0_buf, total_point_num, VAL_SHORT_INTEGER, VAL_THIN_LINE, VAL_EMPTY_SQUARE, VAL_SOLID, 1, VAL_YELLOW);
							PlotY(panelHandle, PANEL_GRAPH_2, p_ch1_buf, total_point_num, VAL_SHORT_INTEGER, VAL_THIN_LINE, VAL_EMPTY_SQUARE, VAL_SOLID, 1, VAL_YELLOW);
						}
						else if (data_src_sel == 2) { //lpf IQ
							if (upload_ch_quantity == 2) {
								PlotY(panelHandle, PANEL_GRAPH, p_ch0_buf, total_point_num, VAL_SHORT_INTEGER, VAL_THIN_LINE, VAL_EMPTY_SQUARE, VAL_SOLID, 1, VAL_WHITE);
								PlotY(panelHandle, PANEL_GRAPH_2, p_ch1_buf, total_point_num, VAL_SHORT_INTEGER, VAL_THIN_LINE, VAL_EMPTY_SQUARE, VAL_SOLID, 1, VAL_YELLOW);
							}
							else if (upload_ch_quantity == 4) {
								if (display_ch_index == 0) {
									PlotY(panelHandle, PANEL_GRAPH, p_ch0_buf, total_point_num, VAL_SHORT_INTEGER, VAL_THIN_LINE, VAL_EMPTY_SQUARE, VAL_SOLID, 1, VAL_WHITE);
									PlotY(panelHandle, PANEL_GRAPH_2, p_ch1_buf, total_point_num, VAL_SHORT_INTEGER, VAL_THIN_LINE, VAL_EMPTY_SQUARE, VAL_SOLID, 1, VAL_YELLOW);
								}
								else if (display_ch_index == 1) {
									PlotY(panelHandle, PANEL_GRAPH, p_ch2_buf, total_point_num, VAL_SHORT_INTEGER, VAL_THIN_LINE, VAL_EMPTY_SQUARE, VAL_SOLID, 1, VAL_WHITE);
									PlotY(panelHandle, PANEL_GRAPH_2, p_ch3_buf, total_point_num, VAL_SHORT_INTEGER, VAL_THIN_LINE, VAL_EMPTY_SQUARE, VAL_SOLID, 1, VAL_YELLOW);
								}
							}
						}
						else { //power&sqrt
							if (upload_ch_quantity == 2) {
								PlotY(panelHandle, PANEL_GRAPH, p_ch0_buf, total_point_num, VAL_SHORT_INTEGER, VAL_THIN_LINE, VAL_EMPTY_SQUARE, VAL_SOLID, 1, VAL_WHITE);

								PlotY(panelHandle, PANEL_GRAPH_2, p_ch1_buf + 0 * total_point_num, total_point_num, VAL_SHORT_INTEGER, VAL_THIN_LINE, VAL_EMPTY_SQUARE, VAL_SOLID, 1, VAL_YELLOW);
								PlotY(panelHandle, PANEL_GRAPH_2, p_ch1_buf + 1 * total_point_num, total_point_num, VAL_SHORT_INTEGER, VAL_THIN_LINE, VAL_EMPTY_SQUARE, VAL_SOLID, 1, VAL_GREEN);
								PlotY(panelHandle, PANEL_GRAPH_2, p_ch1_buf + 2 * total_point_num, total_point_num, VAL_SHORT_INTEGER, VAL_THIN_LINE, VAL_EMPTY_SQUARE, VAL_SOLID, 1, VAL_RED);
								PlotY(panelHandle, PANEL_GRAPH_2, p_ch1_buf + 3 * total_point_num, total_point_num, VAL_SHORT_INTEGER, VAL_THIN_LINE, VAL_EMPTY_SQUARE, VAL_SOLID, 1, VAL_BLUE);
							}
							else if (upload_ch_quantity == 4) {
								if (display_ch_index == 0) {
									PlotY(panelHandle, PANEL_GRAPH, p_ch0_buf, total_point_num, VAL_SHORT_INTEGER, VAL_THIN_LINE, VAL_EMPTY_SQUARE, VAL_SOLID, 1, VAL_WHITE);

									PlotY(panelHandle, PANEL_GRAPH_2, p_ch1_buf + 0 * total_point_num, total_point_num, VAL_SHORT_INTEGER, VAL_THIN_LINE, VAL_EMPTY_SQUARE, VAL_SOLID, 1, VAL_YELLOW);
									PlotY(panelHandle, PANEL_GRAPH_2, p_ch1_buf + 1 * total_point_num, total_point_num, VAL_SHORT_INTEGER, VAL_THIN_LINE, VAL_EMPTY_SQUARE, VAL_SOLID, 1, VAL_GREEN);
									PlotY(panelHandle, PANEL_GRAPH_2, p_ch1_buf + 2 * total_point_num, total_point_num, VAL_SHORT_INTEGER, VAL_THIN_LINE, VAL_EMPTY_SQUARE, VAL_SOLID, 1, VAL_RED);
									PlotY(panelHandle, PANEL_GRAPH_2, p_ch1_buf + 3 * total_point_num, total_point_num, VAL_SHORT_INTEGER, VAL_THIN_LINE, VAL_EMPTY_SQUARE, VAL_SOLID, 1, VAL_BLUE);
								}
								else if (display_ch_index == 1) {
									PlotY(panelHandle, PANEL_GRAPH, p_ch2_buf, total_point_num, VAL_SHORT_INTEGER, VAL_THIN_LINE, VAL_EMPTY_SQUARE, VAL_SOLID, 1, VAL_WHITE);

									PlotY(panelHandle, PANEL_GRAPH_2, p_ch3_buf + 0 * total_point_num, total_point_num, VAL_SHORT_INTEGER, VAL_THIN_LINE, VAL_EMPTY_SQUARE, VAL_SOLID, 1, VAL_YELLOW);
									PlotY(panelHandle, PANEL_GRAPH_2, p_ch3_buf + 1 * total_point_num, total_point_num, VAL_SHORT_INTEGER, VAL_THIN_LINE, VAL_EMPTY_SQUARE, VAL_SOLID, 1, VAL_GREEN);
									PlotY(panelHandle, PANEL_GRAPH_2, p_ch3_buf + 2 * total_point_num, total_point_num, VAL_SHORT_INTEGER, VAL_THIN_LINE, VAL_EMPTY_SQUARE, VAL_SOLID, 1, VAL_RED);
									PlotY(panelHandle, PANEL_GRAPH_2, p_ch3_buf + 3 * total_point_num, total_point_num, VAL_SHORT_INTEGER, VAL_THIN_LINE, VAL_EMPTY_SQUARE, VAL_SOLID, 1, VAL_BLUE);
								}
							}
						}

					}


				}
			}

			if (save_data_en) {
				do {
					Delay(0.001);
					pcie6920_point_num_per_ch_in_buf_query(&point_num_in_buf_per_ch);
				} while (point_num_in_buf_per_ch < (total_point_num * frame_num));

				pcie6920_read_data(total_point_num * frame_num, p_s_buf, &points_per_ch_returned);

				WriteFile(file_handle, (char*)p_s_buf, total_point_num * frame_num * upload_ch_quantity * 2);
			}

		}

		else {
			Delay(0.1);

			recv_enable_cmd = 0;
		}
	}

	return 0;
}

int CVICALLBACK StartStopCB(int panel, int control, int event,
	void* callbackData, int eventData1, int eventData2)
{
	unsigned int start_stop;

	double len_rbw;
	double scan_rate;
	unsigned int pulse_high_width_ns;

	double center_freq_MHz;

	switch (event)
	{
	case EVENT_COMMIT:

		GetCtrlVal(panelHandle, PANEL_BINARYSWITCH_4, &space_time_sel);
		if (space_time_sel) {
			frame_num = 512;
			//SetCtrlAttribute (panelHandle, PANEL_GRAPH_2, ATTR_VISIBLE, 0);
		}
		else {
			GetCtrlVal(panelHandle, PANEL_NUMERIC_20, &frame_num);
			//SetCtrlAttribute (panelHandle, PANEL_GRAPH_2, ATTR_VISIBLE, 1);
		}

		////point num per scan////
		GetCtrlVal(panelHandle, PANEL_NUMERIC_13, &total_point_num);

		pcie6920_set_points_num_per_scan(total_point_num);

		////scan_rate////
		GetCtrlVal(panelHandle, PANEL_NUMERIC, &scan_rate);
		pcie6920_set_scan_rate(scan_rate);

		////pulse_high_width_ns////
		GetCtrlVal(panelHandle, PANEL_NUMERIC_2, &pulse_high_width_ns);
		pcie6920_set_pusle_width(pulse_high_width_ns);

		////center_freq////
		GetCtrlVal(panelHandle, PANEL_NUMERIC_35, &center_freq_MHz);
		pcie6920_set_center_freq(center_freq_MHz * 1000000);

		////data_src_sel////
		GetCtrlVal(panelHandle, PANEL_RADIOBUTTON_6, &sw_dem);
		if (sw_dem) data_src_sel = 2;
		else  GetCtrlVal(panelHandle, PANEL_RING_2, &data_src_sel);

		pcie6920_data_src_sel(data_src_sel);

		////dem_ch_quantity//// 
		GetCtrlVal(panelHandle, PANEL_RING_3, &dem_ch_quantity);
		pcie6920_demodulation_ch_quantity_set(dem_ch_quantity);

		////upload_rate_sel//// 
		GetCtrlVal(panelHandle, PANEL_RING_4, &upload_rate_sel);
		pcie6920_upload_rate_sel(upload_rate_sel);
		len_rbw = 0.4 * upload_rate_sel;

		SetCtrlVal(panelHandle, PANEL_NUMERIC_4, total_point_num * len_rbw / 1000.0);

		//******************************************//
		if ((data_src_sel == 0) || (data_src_sel == 4)) upload_ch_quantity = 2;
		else {
			if (dem_ch_quantity == 1) upload_ch_quantity = 2;
			else if (dem_ch_quantity == 2) upload_ch_quantity = 4;
		}

		if (((upload_ch_quantity == 2) && (total_point_num > 131072)) || (total_point_num % 256 != 0)) {
			MessagePopup("Warning", "双通道模式点数必须小于等于131072,并且是256整数倍!");
			return -1;
		}
		else if (((upload_ch_quantity == 4) && (total_point_num > 65536)) || (total_point_num % 256 != 0)) {
			MessagePopup("Warning", "4通道模式点数必须小于等于65536,并且是256整数倍!");
			return -1;
		}

		SetCtrlVal(panelHandle, PANEL_NUMERIC_34, scan_rate * total_point_num * 2 * upload_ch_quantity / 1024.0 / 1024.0);

		GetCtrlVal(panelHandle, PANEL_RADIOBUTTON_5, &spectrum_en);
		GetCtrlVal(panelHandle, PANEL_BINARYSWITCH_6, &psd_en);
		GetCtrlVal(panelHandle, PANEL_BINARYSWITCH_5, &spectrum_ch_sel);


		GetCtrlVal(panelHandle, PANEL_TOGGLEBUTTON, &start_stop);
		if (start_stop) {

			GetCtrlVal(panelHandle, PANEL_RADIOBUTTON_3, &save_data_en);
			if (save_data_en) {
				start2create_data_file();
				SetCtrlVal(panelHandle, PANEL_LED_2, 1);
			}

			pcie6920_start();
			Delay(0.1);

			p_s_buf = (short*)malloc(total_point_num * upload_ch_quantity * sizeof(short) * frame_num);
			p_ch0_buf = (short*)malloc(total_point_num * sizeof(short) * frame_num);
			p_ch1_buf = (short*)malloc(total_point_num * sizeof(short) * frame_num);
			if (upload_ch_quantity == 4) {
				p_ch2_buf = (short*)malloc(total_point_num * sizeof(short) * frame_num);
				p_ch3_buf = (short*)malloc(total_point_num * sizeof(short) * frame_num);
			}


			if (space_time_sel) {
				p_ch0_space_data_buf = (short*)malloc(sizeof(short) * frame_num);
				p_ch1_space_data_buf = (short*)malloc(sizeof(short) * frame_num);
			}

			if (spectrum_en) p_spectrum = (double*)malloc(total_point_num * sizeof(double));

			if (sw_dem) {
				p_arctan = (double*)malloc(total_point_num * sizeof(double));
				p_power_sqrt = (double*)malloc(total_point_num * 4 * sizeof(double));
			}

			SetCtrlAttribute(panelHandle, PANEL_RING_2, ATTR_DIMMED, 1);
			SetCtrlAttribute(panelHandle, PANEL_RING_3, ATTR_DIMMED, 1);
			SetCtrlAttribute(panelHandle, PANEL_RING_4, ATTR_DIMMED, 1);


			Delay(0.1);
			read_data_enable = 1;

		}
		else {
			read_data_enable = 0;


			do {
				Delay(0.1);
			} while (recv_enable_cmd);

			pcie6920_stop();

			if (p_s_buf != NULL) free(p_s_buf);
			if (p_ch0_buf != NULL) free(p_ch0_buf);
			if (p_ch1_buf != NULL) free(p_ch1_buf);

			if (upload_ch_quantity == 4) {
				if (p_ch2_buf != NULL) free(p_ch2_buf);
				if (p_ch3_buf != NULL) free(p_ch3_buf);;
			}

			if ((p_ch0_space_data_buf != NULL) & space_time_sel)  free(p_ch0_space_data_buf);
			if ((p_ch1_space_data_buf != NULL) & space_time_sel)  free(p_ch1_space_data_buf);

			if (spectrum_en && (p_spectrum != NULL)) free(p_spectrum);

			if (sw_dem) {
				if (p_arctan != NULL) free(p_arctan);
				if (p_power_sqrt != NULL) free(p_power_sqrt);

			}


			SetCtrlAttribute(panelHandle, PANEL_RING_2, ATTR_DIMMED, 0);
			SetCtrlAttribute(panelHandle, PANEL_RING_3, ATTR_DIMMED, 0);
			SetCtrlAttribute(panelHandle, PANEL_RING_4, ATTR_DIMMED, 0);


			if (save_data_en) {
				CloseFile(file_handle);
				SetCtrlVal(panelHandle, PANEL_LED_2, 0);
			}
		}

		break;
	}
	return 0;
}

void start2create_data_file()
{
	int hour, min, sec;
	char str_hour[12] = { 0 }, str_min[12] = { 0 }, str_sec[12] = { 0 };

	GetSystemTime(&hour, &min, &sec);
	if (hour < 10) Fmt(str_hour, "0%d", hour);
	else Fmt(str_hour, "%d", hour);
	if (min < 10) Fmt(str_min, "0%d", min);
	else Fmt(str_min, "%d", min);
	if (sec < 10) Fmt(str_sec, "0%d", sec);
	else Fmt(str_sec, "%d", sec);
	/****************************************************************/
	file_no++;
	Fmt(fileName, "save_data\\%d-%s-%s-%s_D.bin", file_no, str_hour, str_min, str_sec);

	file_handle = OpenFile(fileName, VAL_READ_WRITE, VAL_OPEN_AS_IS, VAL_BINARY);
}

int CVICALLBACK PlayDataCB(int panel, int control, int event,
	void* callbackData, int eventData1, int eventData2)
{
	short* p_play_buf;
	char rt_file_name[MAX_PATHNAME_LEN];
	int i;

	switch (event)
	{
	case EVENT_COMMIT:
		GetCtrlVal(panelHandle, PANEL_NUMERIC_13, &total_point_num);
		p_play_buf = (short*)malloc(total_point_num * sizeof(short) * 4);
		FileSelectPopup("", "*.*", "", "Open Disk", VAL_LOAD_BUTTON, 0, 0, 1, 0, rt_file_name);
		file_handle = OpenFile(rt_file_name, VAL_READ_ONLY, VAL_OPEN_AS_IS, VAL_BINARY);

		for (i = 0; i < 100; i++) {
			ReadFile(file_handle, p_play_buf, total_point_num * 4 * 2);
			DeleteGraphPlot(panelHandle, PANEL_GRAPH, -1, VAL_IMMEDIATE_DRAW);
			PlotY(panelHandle, PANEL_GRAPH, p_play_buf + total_point_num * 0, total_point_num, VAL_SHORT_INTEGER, VAL_THIN_LINE, VAL_EMPTY_SQUARE, VAL_SOLID, 1, VAL_GREEN);
			PlotY(panelHandle, PANEL_GRAPH, p_play_buf + total_point_num * 1, total_point_num, VAL_SHORT_INTEGER, VAL_THIN_LINE, VAL_EMPTY_SQUARE, VAL_SOLID, 1, VAL_RED);
			PlotY(panelHandle, PANEL_GRAPH, p_play_buf + total_point_num * 2, total_point_num, VAL_SHORT_INTEGER, VAL_THIN_LINE, VAL_EMPTY_SQUARE, VAL_SOLID, 1, VAL_BLUE);
			PlotY(panelHandle, PANEL_GRAPH, p_play_buf + total_point_num * 3, total_point_num, VAL_SHORT_INTEGER, VAL_THIN_LINE, VAL_EMPTY_SQUARE, VAL_SOLID, 1, VAL_YELLOW);
			Delay(0.2);
		}
		CloseFile(file_handle);
		MessagePopup("", "Play Data Done!");
		break;
	}
	return 0;
}

int CVICALLBACK WrOffsetDataCB(int panel, int control, int event,
	void* callbackData, int eventData1, int eventData2)
{
	int tmp;
	switch (event)
	{
	case EVENT_COMMIT:
		GetCtrlVal(panelHandle, PANEL_NUMERIC_24, &tmp);
		pcie6920_test_wr_reg(0x90, tmp);
		Delay(1);
		GetCtrlVal(panelHandle, PANEL_NUMERIC_25, &tmp);
		pcie6920_test_wr_reg(0x94, tmp);
		MessagePopup("", "Done!");
		break;
	}
	return 0;
}

int CVICALLBACK RdOffsetDataCB(int panel, int control, int event,
	void* callbackData, int eventData1, int eventData2)
{
	int tmp;
	switch (event)
	{
	case EVENT_COMMIT:
		pcie6920_test_rd_reg(0x90, &tmp);
		SetCtrlVal(panelHandle, PANEL_NUMERIC_27, tmp);
		pcie6920_test_rd_reg(0x94, &tmp);
		SetCtrlVal(panelHandle, PANEL_NUMERIC_26, tmp);
		break;
	}
	return 0;
}


int CVICALLBACK TotalPointNumCheckCB(int panel, int control, int event,
	void* callbackData, int eventData1, int eventData2)
{
	unsigned int total_point_num;
	switch (event)
	{
	case EVENT_COMMIT:
		GetCtrlVal(panelHandle, PANEL_NUMERIC_13, &total_point_num);
		if ((total_point_num % 256) != 0) MessagePopup("Warning", "点数必须为256整数倍，请重新设置！");
		break;
	}
	return 0;
}

void spectrum_analyze(short* p_data_in, unsigned int point_num, unsigned int psd_sel, double sample_rate, double* p_spectrum, double* p_df)
{
	unsigned int i;
	double* p_sig_db;
	//unsigned int windows_type;
	WindowConst constants;
	double* power_spectrum_V2;
	double tmp_db;

	p_sig_db = (double*)malloc(point_num * sizeof(double));
	power_spectrum_V2 = (double*)malloc(sizeof(double) * point_num);

	for (i = 0; i < point_num; i++) {
		p_sig_db[i] = p_data_in[i] / 8192.0;
	}

	ScaledWindow(p_sig_db, point_num, 1, &constants);
	AutoPowerSpectrum(p_sig_db, point_num, 1.0 / sample_rate, power_spectrum_V2, p_df);  //power_spectrum_V2:V2 

	for (i = 0; i < point_num; i++) {
		if (psd_sel) {
			tmp_db = power_spectrum_V2[i] / 50.0 / (*p_df); //W/Hz
			tmp_db = 1000.0 * tmp_db; //mW/Hz
			p_spectrum[i] = 10 * log10(tmp_db + 0.0000000000000000001); //dBm/Hz	
		}
		else {
			tmp_db = power_spectrum_V2[i] / 50.0; //W
			tmp_db = 1000.0 * tmp_db; //mW
			p_spectrum[i] = 10 * log10(tmp_db + 0.0000000000000000001); //dBm
		}
	}

	free(p_sig_db);
	free(power_spectrum_V2);
}

int CVICALLBACK RefClkSrcSelCB(int panel, int control, int event,
	void* callbackData, int eventData1, int eventData2)
{
	int tmp;

	switch (event)
	{
	case EVENT_COMMIT:
		GetCtrlVal(panelHandle, PANEL_BINARYSWITCH_7, &tmp);
		pcie6920_test_wr_reg(0, tmp);
		break;
	}
	return 0;
}

int CVICALLBACK DisplayChDimmSetCB(int panel, int control, int event,
	void* callbackData, int eventData1, int eventData2)
{
	int tmp;

	switch (event)
	{
	case EVENT_COMMIT:
		GetCtrlVal(panelHandle, PANEL_RING_3, &tmp);
		if (tmp == 1) SetCtrlAttribute(panelHandle, PANEL_BINARYSWITCH_8, ATTR_DIMMED, 1);
		else if (tmp == 2) SetCtrlAttribute(panelHandle, PANEL_BINARYSWITCH_8, ATTR_DIMMED, 0);
		break;
	}
	return 0;
}

int CVICALLBACK WR_REG_CB(int panel, int control, int event,
	void* callbackData, int eventData1, int eventData2)
{
	unsigned int reg_addr;
	unsigned int reg_data;

	switch (event)
	{
	case EVENT_COMMIT:
		GetCtrlVal(panelHandle, PANEL_NUMERIC_36, &reg_addr);
		GetCtrlVal(panelHandle, PANEL_NUMERIC_37, &reg_data);

		if ((reg_addr % 4) != 0) {
			MessagePopup("warning", "reg addr 必须是4的整数倍！");
			return 0;
		}

		pcie6920_test_wr_reg(reg_addr, reg_data);

		break;
	}
	return 0;
}

int CVICALLBACK RD_REG_CB(int panel, int control, int event,
	void* callbackData, int eventData1, int eventData2)
{
	unsigned int reg_addr;
	unsigned int reg_data;

	switch (event)
	{
	case EVENT_COMMIT:

		GetCtrlVal(panelHandle, PANEL_NUMERIC_36, &reg_addr);
		if ((reg_addr % 4) != 0) {
			MessagePopup("warning", "reg addr 必须是4的整数倍！");
			return 0;
		}

		pcie6920_test_rd_reg(reg_addr, &reg_data);
		SetCtrlVal(panelHandle, PANEL_NUMERIC_38, reg_data);

		break;
	}
	return 0;
}
