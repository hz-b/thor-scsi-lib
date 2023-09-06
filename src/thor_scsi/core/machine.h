#ifndef _THOR_SCSI_CORE_MACHINE_VOID_H_
#define _THOR_SCSI_CORE_MACHINE_VOID_H_

/**
 * Based heavly on the code of FLAME
 * Or better said:: just a little modified from flame
 */

#include <flame/core/config.h>
// #include <flame/core/util.h>
#include <thor_scsi/core/cell_void.h>
#include <thor_scsi/core/elements_basis.h>
#include <thor_scsi/core/config.h>
#include <ostream>

namespace thor_scsi::core {
	typedef std::vector<std::shared_ptr<CellVoid>> p_elements_t;
	/**
	 * @brief The core simulate Machine engine
	 *
	 * Provides std::vector<ElementVoid*>-like access to individual elements
	 *
	 * @note A Machine instance is reentrant, but not thread-safe.
	 *       Any thread may create a Machine at any time.
	 *       However, each instance should be accessed by a single thread.
	 */
	struct Machine : public boost::noncopyable
	{
		/**
		 * @brief Construct a new Machine
		 * @param c A Config instance, such as that returned by GLPSParser::parse_file().
		 */
		Machine(const Config& c);
	        Machine(const p_elements_t& elements);
	        Machine(p_elements_t& elements);
		~Machine();

		void updateElementList(p_elements_t& elements);

#if 0
		// move constructor
		Machine(Machine&&o) :
			p_elements = std::move(o.p_elements)
			p_conf = std::move(o.p_conf),
			p_trace = std::move(o.p_trace),
			p_simtype = std::move(o.p_simtype),
			p_lookup = std::move(o.p_lookup),
			p_lookup_type = std::move(o.p_lookup_type),
			p_element_infos = std::move(o.p_element_infos),
			p_logger = std::move(o.p_logger)
		{
		}
#endif



		//! Fetch Config used to construct this Machine
		inline const Config& conf() const { return p_conf; }

		/**
		 * @brief Change the configuration of a single element.
		 * @param idx The index of this element
		 * @param c The new Config
		 *
		 * Triggers re-construction of a single element.
		 * An optimization to avoid the overhead of reconstructing
		 * the entire Machine to change a single element.
		 *
		 * @code
		 * Machine M(...);
		 * assert(M.size()>5);
		 * Config e5conf(M[5]->conf());
		 * @endcode
		 */
		void reconfigure(size_t idx, const Config& c);

		//! Return the sim_type string found during construction.
		inline const std::string& simtype() const {return p_simtype;}

		//! The current tracing stream, or NULL.
		inline std::ostream* trace() const {return p_trace;}
		/**
		 * @brief Assign new tracing stream.
		 * @param v new stream or NULL to clear
		 *
		 * The trace stream will be written to during Machine::propagate()
		 * as a debugging aid.
		 */
		void set_trace(std::ostream* v) {p_trace=v;}

	private:

		struct LookupKey {
			std::string name;
			size_t index;
			LookupKey(const std::string& n, size_t i) :name(n), index(i) {}
			bool operator<(const LookupKey& o) const {
				int ord = name.compare(o.name);
				if(ord<0)      return true;
				else if(ord>0) return false;
				else           return index<o.index;
			}
		};

		typedef std::map<LookupKey, std::shared_ptr<CellVoid>> p_lookup_t;
	public:

		//! @return Number of beamline elements
		inline size_t size() const { return p_elements.size(); }

		//! Access a beamline element
		inline std::shared_ptr<CellVoid> operator[](size_t i) { return p_elements[i]; }
		//! Access a beamline element
		inline const std::shared_ptr<CellVoid> operator[](size_t i) const { return p_elements[i]; }

		//! Access a beamline element
		inline std::shared_ptr<CellVoid> at(size_t i) { return p_elements.at(i); }
		//! Access a beamline element
		inline const std::shared_ptr<CellVoid> at(size_t i) const { return p_elements.at(i); }

		//! Beamline element iterator
		typedef p_elements_t::iterator iterator;
		//! Beamline element iterator (const version)
		typedef p_elements_t::const_iterator const_iterator;

		//! Points to the first element
		iterator begin() { return p_elements.begin(); }
		//! Points to the first element
		const_iterator begin() const { return p_elements.begin(); }

		//! Points just after the last element
		iterator end() { return p_elements.end(); }
		//! Points just after the last element
		const_iterator end() const { return p_elements.end(); }

		//! Find the nth element with the given name
		//! @return NULL on failure
		//! A convienence wrapper around equal_range().
		std::shared_ptr<CellVoid> find(const std::string& name, size_t nth=0) {
			p_lookup_t::const_iterator low (p_lookup.lower_bound(LookupKey(name, 0))),
				high(p_lookup.upper_bound(LookupKey(name, (size_t)-1)));
			size_t i=0;
			for(;low!=high;++low,++i) {
				if(i==nth){
					return low->second;
				}
			}
			return nullptr;
		}

		//! iterator for use with equal_range() and equal_range_type()
		typedef value_proxy_iterator<p_lookup_t::iterator> lookup_iterator;

		std::pair<lookup_iterator, lookup_iterator> all_range() {
			return std::make_pair(lookup_iterator(p_lookup.begin()),
					      lookup_iterator(p_lookup.end()));
		}

		//! Return a pair of iterators for the sequence [first, second) of those elements
		//! with the given name.
		std::pair<lookup_iterator, lookup_iterator> equal_range(const std::string& name) {
			p_lookup_t::iterator low (p_lookup.lower_bound(LookupKey(name, 0))),
				high(p_lookup.upper_bound(LookupKey(name, (size_t)-1)));
			return std::make_pair(lookup_iterator(low),
					      lookup_iterator(high));
		}

		//! Return a pair of iterators for the sequence [first, second) of those elements
		//! with the given type name.
		std::pair<lookup_iterator, lookup_iterator> equal_range_type(const std::string& name) {
			p_lookup_t::iterator low (p_lookup_type.lower_bound(LookupKey(name, 0))),
				high(p_lookup_type.upper_bound(LookupKey(name, (size_t)-1)));
			return std::make_pair(lookup_iterator(low),
					      lookup_iterator(high));
		}

		//! Elements with a given name
		p_elements_t elementsWithName(const std::string& name);
		//! Elements with a given name type
		p_elements_t elementsWithNameType(const std::string& name);

	private:
		p_elements_t p_elements;
		p_lookup_t p_lookup; //!< lookup by element instance name
		p_lookup_t p_lookup_type; //!< lookup by element type name
		std::string p_simtype;
		std::ostream* p_trace;
		Config p_conf;


		struct element_builder_t {
			virtual ~element_builder_t() {}
			virtual std::shared_ptr<CellVoid> build(const Config& c) =0;
			virtual void rebuild(std::shared_ptr<CellVoid> o, const Config& c, const size_t idx) =0;
		};
		template<typename Element>
		struct element_builder_impl : public element_builder_t {
			virtual ~element_builder_impl() {}
			std::shared_ptr<CellVoid> build(const Config& c) override final
				{
					auto tmp = std::make_shared<Element>(Element(c));
					return std::static_pointer_cast<CellVoid>(tmp);
				}
			/**
			 * WARNING:
			 *       call to assign missing! (implementation of move ctor).
			 */
			void rebuild(std::shared_ptr<CellVoid> o, const Config& c, const size_t idx) override final
			{
				// std::shared_ptr<CellVoid> N(build(c));
				std::shared_ptr<Element> m = std::dynamic_pointer_cast<Element>(o);
				if(!m)
					throw std::runtime_error("reconfigure() can't change element type");

				// m->assign(N.get());
				m->index = idx; // copy index number
			}
		};

		/**
		 *
		 * Todo:
		 *     Should I name it element or cell info
		 *     Simplification pending ...
		 */
		struct element_info {
			std::string name;
			element_builder_t *builder;
			// typedef std::map<std::string, element_builder_t*> elements_t;
			// elements_t elements;
		};

		/// todo: still required ...
		/// what's the difference to p_element_infos now
		// element_info p_info;

		typedef std::map<std::string, element_info> p_element_infos_t;
		static p_element_infos_t p_element_infos;

		/**
		 * demands that the element name is unique
		 *
		 * Todo:
		 *    investigate howto handle standared development lattices which
		 *    chain standard cells or super cells together
		 */
		static void p_registerElement(const char *ename, element_builder_t* b);

	public:

		/**
		 * @brief Register a new State with the simulation framework.
		 *
		 * The first step to adding a new sim_type.
		 *
		 * @param name The new sim_type name
		 * @throws std::logic_error if name is already registered
		 *
		 * @note This method may be called from any thread at any time.
		 *
		 * @code
		 * struct MyState : public StateBase { ... };
		 * void myReg() {
		 *   Machine::registerState<MyState>("mysimtype");
		 *   ...
		 * @endcode
		 */

		/**
		 * @brief Register a new Element type with the simulation framework
		 *
		 * Add a new element to an existing sim_type (see registerState()).
		 *
		 * @param sname A sim_type name
		 * @param ename The new element type name
		 * @throws std::logic_error if sname has not been registered, or if ename is already registered
		 *
		 * @note This method may be called from any thread at any time.
		 */
		template<typename Element>
		static void registerElement(// const char *sname,   /* NO state */
					    const char *ename){
			p_registerElement(ename, new element_builder_impl<Element>);
		}

		/**
		 * @brief Discard all registered State and Element type information.
		 *
		 * Clears all previous registerations made by registerState() and registerElement().
		 *
		 * Suggested use case is to call just before process exit
		 * so that valgrind doesn't flag these as leaks
		 *
		 * @note This method may be called from any thread at any time.
		 */
		static void registeryCleanup();

		friend std::ostream& operator<<(std::ostream&, const thor_scsi::core::Machine& m);

		struct LogRecord {
			const char * fname;
			unsigned short lnum; // >=64k LoC in one file is already a bug
			unsigned short level;
			std::ostringstream strm;
			LogRecord(const char *fname, unsigned short lnum, unsigned short lvl)
				:fname(fname), lnum(lnum), level(lvl) {}
			~LogRecord();
			template<typename T>
			LogRecord& operator<<(T v)
				{ strm<<v; return *this; }
		private:
			LogRecord(const LogRecord&);
			LogRecord& operator=(const LogRecord&);
		};

		struct Logger {
			virtual ~Logger() {}
			virtual void log(const LogRecord& r)=0;
		};

		static int log_detail;

		static inline bool detail(int lvl) { return log_detail<=lvl; }

		static void set_logger(const std::shared_ptr<Logger>& p);
		static void set_log_level(int lvl);
		static std::shared_ptr<Logger> get_logger(void);
	private:
		static std::shared_ptr<Logger> p_logger;
	};

#define THOR_SCSI_ERROR 40
#define THOR_SCSI_WARN  30
// compatible with python
#define THOR_SCSI_WARNING THOR_SCSI_WARN
#define THOR_SCSI_INFO  20
#define THOR_SCSI_DEBUG 10
// Super verbose logging, a la the rf cavity element
#define THOR_SCSI_FINE  0

#define THOR_SCSI_LOG_ALWAYS(LVL) thor_scsi::core::Machine::LogRecord(__FILE__, __LINE__, THOR_SCSI_##LVL)

#ifndef THOR_SCSI_DISABLE_LOG
#define THOR_SCSI_LOG_CHECK(LVL) UNLIKELY(thor_scsi::core::Machine::detail(THOR_SCSI_##LVL))
#else
#define THOR_SCSI_LOG_CHECK(LVL) (false)
#endif

//! Hook into redirect-able logging.  Use like an std::ostream w/ operator<<
#define THOR_SCSI_LOG(LVL) if(THOR_SCSI_LOG_CHECK(LVL)) THOR_SCSI_LOG_ALWAYS(LVL)

	std::ostream& operator<<(std::ostream&, const Machine& m);

} /* namespace thor scsi */

#endif /* _THOR_SCSI_CORE_MACHINE_VOID_H_ */
/*
 * Local Variables:
 * mode: c++
 * c++-file-style: "python"
 * End:
 */
