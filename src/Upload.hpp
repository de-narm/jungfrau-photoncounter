#pragma once

#include "Config.hpp"
#include "CudaHeader.hpp"
#include "Kernel.hpp"
#include "Pixelmap.hpp"
#include "RingBuffer.hpp"
#include <cmath>

enum ProcessingState { FREE, PROCESSING, READY };

struct deviceData {
    // IDs
    int device;
    int id;
    cudaStream_t str;
    // Event for sequentiell kernel execution
    cudaEvent_t event;
    // Pinned data pointer
    PhotonType* photon_host;
	PhotonType* photon_pinned;
	PhotonSumType* photonsum_pinned;
	//GPU pointer
    PhotonType* photon;
    PhotonSumType* photonsum;
    DataType* data;
    PedestalType* pedestal;
    GainType* gain;
    // Maps
    Gainmap* gain_host;
	Pedestalmap* pedestal_host;
    // State
    ProcessingState state;
    // Number of frames
    std::size_t num_frames;
};

class Uploader {
public:
    Uploader(Gainmap gain, std::size_t numberOfDevices);
    Uploader(const Uploader& other) = delete;
    Uploader& operator=(const Uploader& other) = delete;
    ~Uploader();

    bool isEmpty() const;
    std::size_t upload(Datamap& data, std::size_t offset);
    void uploadPedestaldata(Datamap& data);
    Photonmap download();

	
	void downloadPedestalmap(deviceData* stream);

    void synchronize();
    void printDeviceName() const;

protected:
private:
    RingBuffer<deviceData*> resources;
    static std::vector<deviceData> devices;
    static std::size_t nextFree;
    Gainmap gain;

    static void CUDART_CB callback(cudaStream_t stream, cudaError_t status,
                                   void* data);
    static void CUDART_CB callbackPede(cudaStream_t stream, cudaError_t status,
                                   void* data);

    void initGPUs();
    void freeGPUs();

    void uploadGainmap(struct deviceData stream);

    void downloadGainmap(struct deviceData stream);

    int calcFrames(Datamap& data);
    int calcPedestals(Datamap& data, uint32_t num);
};
