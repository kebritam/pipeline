#pragma once

#include <future>
#include <vector>

namespace pip
{
	class ThreadPool;

	class PipelineElement
	{
	public:
		virtual ~PipelineElement() = default;
	};

	using PipeFunction = PipelineElement * (*)(PipelineElement*);
	using PipeElementGenerator = PipelineElement * (*)();
	using PipeElementDeleter = void (*)(const PipelineElement*);

	class Pipeline
	{
		ThreadPool* m_threadPool;
		std::future<void> m_pipelineLoopFuture;
		bool m_isPipelineStopped;

		PipeElementGenerator m_elementGenerator;
		PipeElementDeleter m_elementDeleter;
		std::vector<PipeFunction> m_pipes;

		void pipelineLoop() const;

	public:
		Pipeline(const Pipeline& _other) = delete;
		Pipeline(Pipeline&& _other) noexcept = delete;
		Pipeline& operator=(const Pipeline& _other) = delete;
		Pipeline& operator=(Pipeline&& _other) noexcept = delete;

		Pipeline(unsigned int _pipeCount, PipeElementGenerator _elementGenerator);
		Pipeline(unsigned int _pipeCount, PipeElementGenerator _elementGenerator, PipeElementDeleter _elementDeleter);
		~Pipeline();

		void AddPipe(PipeFunction _newPipe);
		void Run();
		void Stop();
	};
	
}