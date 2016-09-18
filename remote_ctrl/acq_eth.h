#ifndef ACQ_ETH_H_INCLUDED
#define ACQ_ETH_H_INCLUDED

#include "plugin.h"
#include <netinet/in.h>
#include <netinet/ether.h>
#include "acq_proto.h"
#include "acq_gene.h"

// FIXME : compute FIFO_NB_FRAMES ?
#define FIFO_NB_FRAMES	8192 ///< Number of frames in FIFO : 3spl/frame at 102kHz : min 240ms of buffer
#define FRAME_LOOKUP_CNT 32 ///< number of frame to scan for the waited frame
#define DESYNC_OFFSET 512 ///< if current frame id - waited frame id > DESYNC_OFFSET, we stop keeping synchronization

#define MAX_DEMUX_SPL (16384 / sizeof(sample_t)) ///< demuxing sample block size (16384 bytes in CPU cache)
#define DEMUX_SPL_TRT (MAX_DEMUX_SPL / 128) ///< 128 channels for TRT
#define DEMUX_SPL_IENA (MAX_DEMUX_SPL / 16) ///< 16 channels for iena frames

/// Interval in second between 2 service channel samples (limited by the propagation delay of the commands x nb boards)
#define UPDATE_INTERVAL 2

/// default number of retry for a command
#define DEFAULT_NB_RETRY 2
#define NETCONF_NB_RETRY 10

/// Disconnected mode : enable CMD_STATUS only (detects the bacq)
#define DISCONNECTED_SIMU_FLAGS		(-1 & ~(1 << CMD_STATUS))

/// Number of error service channels
enum {
	SERVICE_ERR_CORRECTED,
	SERVICE_ERR_LOST,
	NB_SERVICE_ERR,
};

struct board_chan_t {
	enum chan_cap_t cap;			///< Channel capabilities
	enum chan_opt_t conf;			///< Channel config
	struct teds_t teds;			///< Channel TEDS data
};

/// Data for a CND board
struct board_cnd_t {
	int modified;	/// Set to TRUE to reconfigure the board (ACQCONF)
	enum board_cap_t cap;
	enum board_opt_t conf;
	int nb_chan;
	struct board_chan_t chan[];	/// Array of nb_chan channels
};

enum {
	DAC_AC,
	DAC_DC,
	NB_DAC
};

/// board models
enum {
	MODEL_TRT,     ///< trt board with cnd_acq acquisition boards
	MODEL_BACQ_V1, ///< bacq v1 (97kHz max acq frequency, SPI ADC interface)
	MODEL_BACQ_V2, ///< bacq v2 (144kHz max acq frequency, Frame-Sync ADC interface)
	NB_BOARD_MODEL ///< number of boards model
};

struct DAC_t {
	float ref_voltage;	///< Reference voltage of the DAC
	int bits;		///< Bits size of the DAC, 0 if the dac is not available
	float gain;		///< Analog Gain after the DAC
	float frequency;	///< Frequency of the DAC (for AC generation)
};

/// Maximum number of CND boards on a ACQ
#define MAX_CND_PER_CNDACQ	2
#define MAX_CND_ACQ		2

/// Data for a CND_ACQ board
struct board_cnd_acq_t {
	char serial[BOARD_SERIAL_LEN];
	time_t next_retry;      /// if not 0 : the board doesn't answer to commands and no command will be sent until this time
	char board_address;		/// CND_ACQ address, conditionners have the following addresses
	int invalid_ech;		/// Number of invalid samples at begining of the acquisition
	int nb_chan;			/// Number of channels on this CND_ACQ
	int nb_cnd;             /// Number of conditionner including this
	struct DAC_t dac[NB_DAC];	///< bits != 0 if the board can generate (CA64)
	struct board_cnd_t *cnd[MAX_CND_PER_CNDACQ]; 	/// Array of pointers to nb_cnd conditioners, cnd[0] is this cnd_acq
};

/// Data for a TRE board
struct board_tre_t {
	int modified;	/// Set to TRUE to reconfigure the board (TXCONF)
	enum txconf_opt_t conf;
	int nb_acq;					/// Number of CND_ACQ boards
	int nb_adc;					/// Number of ADC on each CND_ACQ board
	struct DAC_t rec_dac[NB_DAC];	///< bits != 0 if REC is available, else generate on CND_ACQ DAC info
	struct board_cnd_acq_t cnd_acq[MAX_CND_ACQ];	/// Array of nb_acq CND_ACQ boards
};

/// Data for a TRT board
struct board_trt_t {
	int32_t model;      ///< model of the board see NB_BOARD_MODEL
	char prefix[16];    /// Prefix to the default channel names
	int id;	            /// Id of this TRT (issued from the MAC or IP address?)
	struct ether_addr mac_addr;	/// Board MAC address
	struct sockaddr_in board_addr;	/// Board IP + Port
	struct sockaddr_in server_addr;	/// Server IP + Data Port
	struct in_addr bcast_ip;	/// Broadcast IP to use
	enum who_cap_flags_t flags;	/// TRT clk flags
	int mtu;			/// Board MTU
	uint32_t clk_reg;		/// 32bits config register of the clock
	float frequency;		/// Current sample frequency
	struct board_tre_t tre;		/// TRE Data
};

enum demux_state_t {
	RUNNING = 1,	///< demux thread is active
	MISSING = 2,	///< missing frames mode
	DESYNC = 4	///< Synchro is lost
};

enum {
    ERROR_NOT_FOUND = 32,
    ERROR_BOARD_NOT_READY = 64,
    ERROR_UNKNOWN = 128
};

#define is_DMR_board(inst_data) (inst_data->trt.model == MODEL_TRT)
#define is_CA64_board(inst_data) (inst_data->simu_data.dmr_board == ACQ_BOARD_CA64 \
	|| inst_data->simu_data.dmr_board == ACQ_BOARD_Z10_1_CA64 \
	|| inst_data->simu_data.dmr_board == ACQ_BOARD_Z10_2_CA64)

/// Type of DMR boards emulated
enum acq_board_name_t {
	ACQ_BOARD_CND_ACQ,
	ACQ_BOARD_CA64,
	ACQ_BOARD_Z10_1_CND_ACQ,
	ACQ_BOARD_Z10_2_CND_ACQ,
    ACQ_BOARD_Z10_1_CA64,
    ACQ_BOARD_Z10_2_CA64,
	ACQ_BOARD_OCARINA,
	NB_ACQ_BOARD_DMR, // FIXME = ACQ_BOARD_UNUSED
};

enum simu_status_t {
	BOARDSIMU_NONE = 0,	///< No board simulation : board available
	BOARDSIMU_WAIT,		///< Board simulated, plugin in error
};

struct ptp_demux_t { // Used in PTP mode
	uint64_t last_ptp_ticks;	/// PTP date (ticks) of the  last frame used
	uint64_t spl_period_num;	/// Sample period in ticks (numerator)
	uint64_t spl_period_den;	/// Sample period in ticks (denominator)

	uint64_t start_date_ticks;	/// Date of start in ticks, 0xF...F if we not yet known
    uint64_t start_spl;		/// Date of start in samples
    uint64_t next_datation_ticks;	/// Last datation given to the server
    uint64_t datation_interval_ticks;	/// Last datation given to the server
    uint frequency;		/// PTP clock frequency, in Hz
};

#define NB_THREADS 3

struct inst_data_t {
	int sock_data;		/// UDP socket to receive Data
	int sock_cmd;		/// UDP socket to send and receive commands
    int nb_retry;       /// number of retry for this board commands
	mutex_t mutex_sock;	/// Locked while waiting an answer
	int global_sample_size;	/// Sample size in bytes (SAMPLE_16 or SAMPLE_24) on all channels
	float ref_ADC;		/// Ref voltage of the ADC

	int nb_chan; 			/// Number of data channels
	int nb_service_chan;		/// Number of service channels
	struct channel_t *channels; 	/// pointer to board channels
	struct conditioner_t *cond;	/// channel conditioners
	char **chan_desc;		/// channel descriptions
	struct {
		struct paramSet_t *sets;		/// Channel param sets
		struct param_descriptor_t *desc;	/// Channel param descriptors
	} chan_params;
	struct cond_gain_t *manuf_gains;	/// Array of nb_gain_max manufacture gains per channel
	int nb_gain_max;		/// Max number of selectable gains

	struct {
		int nb_brd;			/// Number of boards with status command
		char dst_status[32];		/// List of board dest to status (service channels)
		char dst_first_chan[32];	/// First service channel for each dest board
		sample_t *value;	/// Pointer to an array of nb_service_chan last samples of the service channels
	} service;

	struct board_trt_t trt;		/// boards
    int is_ready;
	union {
		struct {
			pthread_t thread_udp, thread_demux, thread_service;
		};
		pthread_t threads[NB_THREADS];
	};
	struct {
		enum demux_state_t state; /// demux state
		sem_t sem;
		char *fifo_buff;	/// udp Rx frame buffer
		int fifo_size;		/// fifo size in bytes
		struct fifo_frame_t *fifo_rd; /// fifo read pointer
		struct fifo_frame_t *fifo_wr; /// fifo write pointer
		int tre_per_frame;	/// Number of TRE frames per UDP data frame
		int frame_size;
		int start_size; /// minimum number of bytes in fifo to start demux
		int min_size; /// minimum number of bytes in fifo to continue demux
		int invalid_spl;	/// Number of invalid samples remaining (inited when acq starts)
		struct { // Used in non PTP mode
			uint16_t waited_frame_id;	/// Waited frame to demux
			uint8_t tre_frame_id;	///< Waited TRE frame to demux
		} frame;
		struct ptp_demux_t ptp;
	} demux;
	struct {	/// Save the current gen params to keep them if reconfiguring the boards
		enum gen_type_t type;	/// Current generation type
		enum chan_opt_t shunts;	/// set COPT_SHUNT_H and/or COPT_SHUNT_L to enable the generation with shunts
		enum special_dest_id_t dest;	/// Board currently generating
		int types_gen;			/// Array of flags for each generation available (1 << gen_type_t)
	} generation;
	int cmd_simu_flags;		///< Set flags for commands simulated (each flag is 1 << cmd_type_t)
	struct {
		enum simu_status_t status;	///< Current simulation status
		int dmr_board;		///< Type of DMR board to simulate (if used)
		char glob_param_name[20];
		struct param_buf_t *data;
		int has_changed;
	} simu_data;		///< Data used for simulation mode
};

/** Get a status from a board
 * @param inst_data inst_data
 * @param dst destination of th command
 * @param is_status_field return answer to status_field if TRUE, else return answer to status
 *
 * @return allocated received answer, or NULL if failed
*/
void *ask_status(struct inst_data_t *inst_data, int dst, int is_status_field);

/// Try to leave the simulation mode (if board connected)
void reconfigure_autodetected_board(struct plugin_t *inst);

int acq_threads_start(struct plugin_t *inst);
int acq_threads_stop(struct plugin_t *inst);

/** Initialize the buffer of saved data (load currently saved buffer)
 * @param inst this plugin
 * @param mac Plugin unique identification string
*/
void init_simulation_data(struct plugin_t *inst, char *mac);

/// Save the modified simuation data
void save_simulation_data(struct plugin_t *inst);

/// Save and close the board data
void close_simulation_data(struct plugin_t *inst);

/** Add or update a board answer
 * @param inst_data
 * @param answer the raw answer received from the board
 * @param value_id id of a value (for READVALUE commands only)
*/
void add_simulation_data(struct inst_data_t *inst_data, void *answer, int value_id);

/** Get a saved answer to a command
 * @param inst_data
 * @param command the command id
 * @param board the board destination of the command
 * @param value_id id of a value (for READVALUE commands only)
 *
 * @return An allocated buffer with the raw data (NULL if no data available)
*/
void *retrieve_simulation_data(struct inst_data_t *inst_data, enum cmd_type_t command, int board, int value_id);

#endif // ACQ_ETH_H_INCLUDED
