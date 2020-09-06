/***************************************************************
 * Author: Quirijn Bouts (www.qbouts.com | github.com/qbouts)  *
 * LICENSE: MIT                                                *
 ***************************************************************/

#include <gtest/gtest.h>

#include <functional>
#include <memory>
#include <stdexcept>
#include <string>

#include "ParameterMap.h"

namespace {
using qbouts::ParameterMap;
using qbouts::detail::static_for;

class ParameterMapTestSuite : public ::testing::Test {};

TEST_F(ParameterMapTestSuite, NParameterParameterMapCanBeCreatedWithExactlyNParamNames) {
	ParameterMap<int, bool, const std::string&> map{"myInt", "enabled", "name"};
}

TEST_F(ParameterMapTestSuite, ParametersCanBeSetByName) {
	ParameterMap<int, bool, const std::string&> map{"myInt", "enabled", "name"};
	map.set("myInt", 3);
	map.set("enabled", true);
	map.set("name", "Homer Simpson");
}

TEST_F(ParameterMapTestSuite, ParametersCanBeSetByCompileTimeIndex) {
	ParameterMap<int, bool, const std::string&> map{"myInt", "enabled", "name"};
	map.set<0>(3);
	map.set<1>(true);
	map.set<2>("Homer Simpson");
}

TEST_F(ParameterMapTestSuite, ParametersCanBeSetByRuntimeIndex) {
	ParameterMap<int, bool, const std::string&> map{"myInt", "enabled", "name"};
	map.set(0, 3);
	map.set(1, true);
	map.set(2, "Homer Simpson");
}

TEST_F(ParameterMapTestSuite, SettingParamerByTooHighRuntimeIndexThrowsOutOfRange) {
	ParameterMap<int, bool, const std::string&> map{"myInt", "enabled", "name"};
	EXPECT_THROW(map.set(3, 3), std::out_of_range);
}

TEST_F(ParameterMapTestSuite, SettingParameterWithIncorrectNameThrowsInvalidArgument) {
	ParameterMap<int, bool, const std::string&> map{"myInt", "enabled", "name"};
	EXPECT_THROW(map.set("not_myInt", 3), std::invalid_argument);
	EXPECT_THROW(map.set("not_enabled", true), std::invalid_argument);
	EXPECT_THROW(map.set("not_name", "Homer Simpson"), std::invalid_argument);
}
TEST_F(ParameterMapTestSuite, GettingParamerByTooHighRuntimeIndexThrowsOutOfRange) {
	ParameterMap<int, bool, const std::string&> map{"myInt", "enabled", "name"};
	EXPECT_THROW([[maybe_unused]] auto dummy = map.get<int>(3), std::out_of_range);
}
TEST_F(ParameterMapTestSuite, PreviouslySetParametersCanBeRetrievedByName) {
	ParameterMap<int, bool, const std::string&> map{"myInt", "enabled", "name"};
	map.set("myInt", 3);
	map.set("enabled", true);
	map.set("name", "Homer Simpson");

	[[maybe_unused]] auto myInt = map.get<int>("myInt");
	[[maybe_unused]] auto enabled = map.get<bool>("enabled");
	[[maybe_unused]] auto name = map.get<std::string>("name");
}

TEST_F(ParameterMapTestSuite, RetrievingsNonSetParameterByNameThrowsRuntimeError) {
	ParameterMap<int, bool, const std::string&> map{"myInt", "enabled", "name"};

	EXPECT_THROW([[maybe_unused]] auto myInt = map.get<int>("myInt"), std::runtime_error);
	EXPECT_THROW([[maybe_unused]] auto enabled = map.get<bool>("enabled"), std::runtime_error);
	EXPECT_THROW([[maybe_unused]] auto name = map.get<std::string>("name"), std::runtime_error);
}

TEST_F(ParameterMapTestSuite, GettingParameterByNameWhichWasPreviouslySetByNameReturnsCorrectValue) {
	ParameterMap<int, bool, const std::string&> map{"myInt", "enabled", "name"};
	map.set("myInt", 3);
	map.set("enabled", true);
	map.set("name", "Homer Simpson");

	EXPECT_EQ(map.get<int>("myInt"), 3);
	EXPECT_EQ(map.get<bool>("enabled"), true);
	EXPECT_EQ(map.get<std::string>("name"), "Homer Simpson");
}

TEST_F(ParameterMapTestSuite, IsSetByNameReturnsCorrectValue) {
	ParameterMap<int, bool, const std::string&> map{"myInt", "enabled", "name"};

	EXPECT_FALSE(map.is_set("myInt"));
	EXPECT_FALSE(map.is_set("enabled"));
	EXPECT_FALSE(map.is_set("name"));

	map.set("myInt", 3);
	map.set("name", "Homer Simpson");

	EXPECT_TRUE(map.is_set("myInt"));
	EXPECT_FALSE(map.is_set("enabled"));
	EXPECT_TRUE(map.is_set("name"));
}

TEST_F(ParameterMapTestSuite, IsSetByRuntimeIndexReturnsCorrectValue) {
	ParameterMap<int, bool, const std::string&> map{"myInt", "enabled", "name"};

	EXPECT_FALSE(map.is_set(0));
	EXPECT_FALSE(map.is_set(1));
	EXPECT_FALSE(map.is_set(2));

	map.set("myInt", 3);
	map.set("name", "Homer Simpson");

	EXPECT_TRUE(map.is_set(0));
	EXPECT_FALSE(map.is_set(1));
	EXPECT_TRUE(map.is_set(2));
}

TEST_F(ParameterMapTestSuite, IsSetByCompiletTimeIndexReturnsCorrectValue) {
	ParameterMap<int, bool, const std::string&> map{"myInt", "enabled", "name"};

	EXPECT_FALSE(map.is_set<0>());
	EXPECT_FALSE(map.is_set<1>());
	EXPECT_FALSE(map.is_set<2>());

	map.set("myInt", 3);
	map.set("name", "Homer Simpson");

	EXPECT_TRUE(map.is_set<0>());
	EXPECT_FALSE(map.is_set<1>());
	EXPECT_TRUE(map.is_set<2>());
}

TEST_F(ParameterMapTestSuite, SettingParameterWhichWasSetBeforeUpdatesItsValue) {
	ParameterMap<int, bool, const std::string&> map{"myInt", "enabled", "name"};
	map.set("myInt", 3);
	map.set("enabled", true);
	map.set("name", "Homer Simpson");

	EXPECT_EQ(map.get<int>("myInt"), 3);
	EXPECT_EQ(map.get<bool>("enabled"), true);
	EXPECT_EQ(map.get<std::string>("name"), "Homer Simpson");

	map.set("myInt", 6);
	map.set("enabled", false);
	map.set("name", "Marge Simpson");

	EXPECT_EQ(map.get<int>("myInt"), 6);
	EXPECT_EQ(map.get<bool>("enabled"), false);
	EXPECT_EQ(map.get<std::string>("name"), "Marge Simpson");
}

TEST_F(ParameterMapTestSuite, SubmittingParametersToFunctionWhenNotAllHaveBeenSetThrowsRuntimeError) {
	ParameterMap<int, bool, const std::string&> map{"myInt", "enabled", "name"};
	EXPECT_THROW(map.submit([](int, bool, const std::string&) { return 4; }), std::runtime_error);
	map.set("myInt", 6);
	map.set("enabled", false);
	EXPECT_THROW(map.submit([](int, bool, const std::string&) { return 4; }), std::runtime_error);
}

TEST_F(ParameterMapTestSuite, SubmittingParametersToFunctionCallsFunctionWithCorrectParameters) {
	ParameterMap<int, bool, const std::string&> map{"myInt", "enabled", "name"};

	map.set("myInt", 6);
	map.set("enabled", true);
	map.set("name", "Homer Simpson");

	int myInt = 0;
	bool enabled = false;
	std::string name;
	auto testfunc = [&](int a, bool b, const std::string& c) {
		myInt = a;
		enabled = b;
		name = c;
		return 7;
	};

	EXPECT_EQ(map.submit(testfunc), 7);
	EXPECT_EQ(myInt, 6);
	EXPECT_EQ(enabled, true);
	EXPECT_EQ(name, "Homer Simpson");
}

enum class GetMemberType { BY_NAME, BY_RT_INDEX, BY_CT_INDEX };
enum class SetMemberType { BY_NAME, BY_RT_INDEX, BY_CT_INDEX };

std::string to_string(GetMemberType get) {
	switch (get) {
		case GetMemberType::BY_NAME: return "ByName";
		case GetMemberType::BY_RT_INDEX: return "ByRuntimeIndex";
		case GetMemberType::BY_CT_INDEX: return "ByCompiletimeIndex";
	}
	return "";
}
std::string to_string(SetMemberType set) {
	switch (set) {
		case SetMemberType::BY_NAME: return "ByName";
		case SetMemberType::BY_RT_INDEX: return "ByRuntimeIndex";
		case SetMemberType::BY_CT_INDEX: return "ByCompiletimeIndex";
	}
	return "";
}

class ParameterMapSetGetTestSuite : public testing::TestWithParam<std::tuple<GetMemberType, SetMemberType>> {
public:
	using param_tuple_t = std::tuple<int, bool, std::string>;
	static constexpr size_t n_params = std::tuple_size_v<param_tuple_t>;

	ParameterMapSetGetTestSuite() : m_param_map("Param0", "Param1", "Param2") {}
	void Get(GetMemberType method) {
		switch (method) {
			case GetMemberType::BY_NAME:
				static_for<0, n_params>([&](auto i) {
					std::get<i.value>(m_output) = m_param_map.get<std::tuple_element_t<i.value, param_tuple_t>>(
							std::string{"Param"} + std::to_string(i.value));
				});
				break;
			case GetMemberType::BY_RT_INDEX:
				static_for<0, n_params>([&](auto i) {
					std::get<i.value>(m_output) = m_param_map.get<std::tuple_element_t<i.value, param_tuple_t>>(i.value);
				});
				break;
			case GetMemberType::BY_CT_INDEX:
				static_for<0, n_params>([&](auto i) { std::get<i.value>(m_output) = m_param_map.get<i.value>(); });
				break;
		}
	}
	void Set(SetMemberType method) {
		switch (method) {
			case SetMemberType::BY_NAME:
				static_for<0, n_params>([&](auto i) {
					m_param_map.set(std::string{"Param"} + std::to_string(i.value), std::get<i.value>(m_input));
				});
				break;
			case SetMemberType::BY_RT_INDEX:
				static_for<0, n_params>([&](auto i) { m_param_map.set(i.value, std::get<i.value>(m_input)); });
				break;
			case SetMemberType::BY_CT_INDEX:
				static_for<0, n_params>([&](auto i) { m_param_map.set<i.value>(std::get<i.value>(m_input)); });
				break;
		}
	}

	void IsSet(GetMemberType method) {
		switch (method) {
			case GetMemberType::BY_NAME:
				static_for<0, n_params>([&](auto i) {
					std::get<i.value>(m_is_set_output) = m_param_map.is_set(std::string{"Param"} + std::to_string(i.value));
				});
				break;
			case GetMemberType::BY_RT_INDEX:
				static_for<0, n_params>([&](auto i) { std::get<i.value>(m_is_set_output) = m_param_map.is_set(i.value); });
				break;
			case GetMemberType::BY_CT_INDEX:
				static_for<0, n_params>([&](auto i) { std::get<i.value>(m_is_set_output) = m_param_map.is_set<i.value>(); });
				break;
		}
	}

protected:
	ParameterMap<int, bool, const std::string&> m_param_map;
	param_tuple_t m_input;
	param_tuple_t m_output;
	std::array<bool, std::tuple_size_v<param_tuple_t>> m_is_set_output{};
};

TEST_P(ParameterMapSetGetTestSuite, SetGetRetrievesCorrectValues) {
	m_input = param_tuple_t{42, true, "Hello!"};
	Set(std::get<SetMemberType>(GetParam()));
	Get(std::get<GetMemberType>(GetParam()));
	EXPECT_EQ(m_output, m_input);
}
TEST_P(ParameterMapSetGetTestSuite, SetSetGetRetrievesCorrectValues) {
	m_input = param_tuple_t{42, true, "Hello!"};
	Set(std::get<SetMemberType>(GetParam()));
	m_input = param_tuple_t{25, false, "Good bye!"};
	Set(std::get<SetMemberType>(GetParam()));
	Get(std::get<GetMemberType>(GetParam()));
	EXPECT_EQ(m_output, m_input);
}

TEST_P(ParameterMapSetGetTestSuite, RetrievingNonSetParameterThrowsRuntimeError) {
	EXPECT_THROW(Get(std::get<GetMemberType>(GetParam())), std::runtime_error);
}
TEST_P(ParameterMapSetGetTestSuite, RetrievingSetButLaterClearedParameterThrowsRuntimeError) {
	m_input = param_tuple_t{42, true, "Hello!"};
	Set(std::get<SetMemberType>(GetParam()));
	m_param_map.clear();
	EXPECT_THROW(Get(std::get<GetMemberType>(GetParam())), std::runtime_error);
}
TEST_P(ParameterMapSetGetTestSuite, VerifyIsSetGivesCorrectOutput) {
	m_input = param_tuple_t{42, true, "Hello!"};
	std::array<bool, std::tuple_size_v<param_tuple_t>> not_set_output{};
	not_set_output.fill(false);
	std::array<bool, std::tuple_size_v<param_tuple_t>> set_output{};
	set_output.fill(true);

	IsSet(std::get<GetMemberType>(GetParam()));
	EXPECT_EQ(m_is_set_output, not_set_output);

	Set(std::get<SetMemberType>(GetParam()));

	IsSet(std::get<GetMemberType>(GetParam()));
	EXPECT_EQ(m_is_set_output, set_output);

	Set(std::get<SetMemberType>(GetParam()));

	IsSet(std::get<GetMemberType>(GetParam()));
	EXPECT_EQ(m_is_set_output, set_output);

	m_param_map.clear();

	IsSet(std::get<GetMemberType>(GetParam()));
	EXPECT_EQ(m_is_set_output, not_set_output);

	m_param_map.clear();

	IsSet(std::get<GetMemberType>(GetParam()));
	EXPECT_EQ(m_is_set_output, not_set_output);
}


INSTANTIATE_TEST_SUITE_P(
		SetGetTests,
		ParameterMapSetGetTestSuite,
		testing::Combine(testing::Values(GetMemberType::BY_NAME, GetMemberType::BY_RT_INDEX, GetMemberType::BY_CT_INDEX),
										 testing::Values(SetMemberType::BY_NAME, SetMemberType::BY_RT_INDEX, SetMemberType::BY_CT_INDEX)),
		[](const testing::TestParamInfo<ParameterMapSetGetTestSuite::ParamType>& param_info) {
			return std::string{"Get"} + to_string(std::get<GetMemberType>(param_info.param)) + "Set" +
						 to_string(std::get<SetMemberType>(param_info.param));
		});
}  // namespace
