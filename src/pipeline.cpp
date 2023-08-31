#include "Pipeline.h"

#include "ThreadPool.h"

using namespace pip;

Pipeline::Pipeline(const unsigned int _pipeCount, const PipeElementGenerator _elementGenerator)
	: m_threadPool(new ThreadPool(_pipeCount))
	, m_isPipelineStopped(true)
	, m_elementGenerator(_elementGenerator)
	, m_elementDeleter([](const PipelineElement* _elem) { delete _elem; })
{
	m_pipes.reserve(_pipeCount - 1);
}

Pipeline::Pipeline(const unsigned int _pipeCount, const PipeElementGenerator _elementGenerator, const PipeElementDeleter _elementDeleter)
	: Pipeline(_pipeCount, _elementGenerator)
{
	m_elementDeleter = _elementDeleter;
}

Pipeline::~Pipeline()
{
	if (m_pipelineLoopFuture.valid())
		m_pipelineLoopFuture.wait();
	delete m_threadPool;
}

void Pipeline::AddPipe(const PipeFunction _newPipe)
{
	if (!m_isPipelineStopped)
		Stop();
	m_pipes.push_back(_newPipe);
}

void Pipeline::Stop()
{
	m_isPipelineStopped = true;
	if (!m_pipelineLoopFuture.valid())
		throw std::runtime_error("Pipeline is not running");
	
	m_pipelineLoopFuture.get();
}

void Pipeline::Run()
{
	if (m_pipelineLoopFuture.valid() && m_pipelineLoopFuture.wait_for(std::chrono::seconds(0)) != std::future_status::ready)
		throw std::runtime_error("Pipeline is already running");
	m_isPipelineStopped = false;
	m_pipelineLoopFuture = std::async(std::launch::async, [this] { pipelineLoop(); });
}

void Pipeline::pipelineLoop() const
{
	std::vector<PipelineElement*> pipelineElements(m_pipes.size() + 1, nullptr);

	while (!m_isPipelineStopped)
	{
		std::vector<std::future<PipelineElement*>> pipeFutures;
		pipeFutures.reserve(m_pipes.size() + 1);

		std::future<PipelineElement*> firstPipeFuture =
			m_threadPool->Enqueue([this]() { return m_elementGenerator(); });
		pipeFutures.push_back(std::move(firstPipeFuture));

		for (size_t idx = 0 ; idx < pipelineElements.size() - 1 ; ++idx)
		{
			std::future<PipelineElement*> pipeFuture =
				m_threadPool->Enqueue([&, idx]() { return m_pipes[idx](pipelineElements[idx + 1]); });
			pipeFutures.push_back(std::move(pipeFuture));
		}

		m_elementDeleter(pipeFutures[pipeFutures.size() - 1].get());
		for (size_t idx = pipelineElements.size() - 1; idx > 0; --idx)
			pipelineElements[idx] = pipeFutures[idx - 1].get();
	}
}
