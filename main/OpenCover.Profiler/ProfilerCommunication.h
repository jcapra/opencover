//
// OpenCover - S Wilde
//
// This source code is released under the MIT License; see the accompanying license file.
//
#pragma once

#include "Synchronization.h"
#include "SharedMemory.h"
#include "Messages.h"

#include <ppl.h>
#include <concurrent_queue.h>

/// <summary>Handles communication back to the profiler host</summary>
/// <remarks>Currently this is handled by using the WebServices API</remarks>
class ProfilerCommunication
{
private:

public:
    ProfilerCommunication();
    ~ProfilerCommunication(void);
    bool Initialise(TCHAR* key, TCHAR *ns);
    void Stop();

public:
    bool TrackAssembly(WCHAR* pModulePath, WCHAR* pAssemblyName);
    bool GetPoints(mdToken functionToken, WCHAR* pModulePath, WCHAR* pAssemblyName, std::vector<SequencePoint> &seqPoints, std::vector<BranchPoint> &brPoints);
    bool TrackMethod(mdToken functionToken, WCHAR* pModulePath, WCHAR* pAssemblyName, ULONG &uniqueId);
    inline void AddVisitPoint(ULONG uniqueId) { if (uniqueId!=0) m_queue.push(uniqueId | IT_VisitPoint); }
    inline void AddTestEnterPoint(ULONG uniqueId) { if (uniqueId!=0) m_queue.push(uniqueId | IT_MethodEnter); }
    inline void AddTestLeavePoint(ULONG uniqueId) { if (uniqueId!=0) m_queue.push(uniqueId | IT_MethodLeave); }
    inline void AddTestTailcallPoint(ULONG uniqueId) { if (uniqueId!=0) m_queue.push(uniqueId | IT_MethodTailcall); }

private:
    void SendVisitPoints();
    bool GetSequencePoints(mdToken functionToken, WCHAR* pModulePath, WCHAR* pAssemblyName, std::vector<SequencePoint> &points);
    bool GetBranchPoints(mdToken functionToken, WCHAR* pModulePath, WCHAR* pAssemblyName, std::vector<BranchPoint> &points);

private:
    tstring m_key;
    tstring m_namespace;

    template<class BR, class PR>
    void RequestInformation(BR buildRequest, PR processResults);

private:
    CMutex m_mutexCommunication;
    CSharedMemory m_memoryCommunication;
    CEvent m_eventProfilerRequestsInformation;
    CEvent m_eventInformationReadyForProfiler;
    MSG_Union *m_pMSG;
    CEvent m_eventInformationReadByProfiler;

private:
    CMutex m_mutexResults;
    CSharedMemory m_memoryResults;
    CEvent m_eventProfilerHasResults;
    CEvent m_eventResultsHaveBeenReceived;
    MSG_SendVisitPoints_Request *m_pVisitPoints;

private:
    static DWORD WINAPI QueueProcessingThread(LPVOID lpParam);
    void ProcessResults();
    ATL::CComAutoCriticalSection m_critResults;
    Concurrency::concurrent_queue<ULONG> m_queue;
    HANDLE m_hQueueThread;

};

