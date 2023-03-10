// Copyright 2022 Andi Hellmund
//
//  Licensed under the Apache License, Version 2.0 (the "License");
//  you may not use this file except in compliance with the License.
//  You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
//  Unless required by applicable law or agreed to in writing, software
//  distributed under the License is distributed on an "AS IS" BASIS,
//  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//  See the License for the specific language governing permissions and
//  limitations under the License.

#ifndef DATA_CRUNCHING_INTERNAL_ARGPARSE_HPP
#define DATA_CRUNCHING_INTERNAL_ARGPARSE_HPP

#include <string>
#include <deque>
#include <sstream>

#include "data_crunching/internal/fixed_string.hpp"
#include "data_crunching/namedtuple.hpp"
#include "data_crunching/internal/type_conversion.hpp"
#include "data_crunching/internal/type_list.hpp"
#include "data_crunching/internal/utils.hpp"

namespace dacr {

namespace internal {

template <FixedString Name, typename Type>
class ArgImpl;

// ############################################################################
// Class: Spec classes
// ############################################################################
struct Mnemonic {
    std::string short_arg{};
};

struct Positional {
};

struct Required {  
};

struct Help {
    std::string text{};
};

template <typename T>
struct Optional {
    T value;
};

struct Store {
    bool value{true};
};

// ############################################################################
// Utility: Get Mnemonic
// ############################################################################
std::optional<std::string> getMnemonic () {
    return std::nullopt;
}

template <typename FirstSpec, typename ...RestSpecs>
std::optional<std::string> getMnemonic (FirstSpec first_spec, RestSpecs ...rest_specs) {
    return getMnemonic(rest_specs...);
}

template <typename ...RestSpecs>
std::optional<std::string> getMnemonic(Mnemonic mnemonic, RestSpecs ...rest_specs) {
    return mnemonic.short_arg;
}

// ############################################################################
// Utility: Get Help
// ############################################################################
std::optional<std::string> getHelp () {
    return std::nullopt;
}

template <typename FirstSpec, typename ...RestSpecs>
std::optional<std::string> getHelp (FirstSpec first_spec, RestSpecs ...rest_specs) {
    return getHelp(rest_specs...);
}

template <typename ...RestSpecs>
std::optional<std::string> getHelp(Help help, RestSpecs ...rest_specs) {
    return help.text;
}

// ############################################################################
// Utility: Get Optional
// ############################################################################
auto getOptional () {
    return std::nullopt;
}

template <typename FirstSpec, typename ...RestSpecs>
auto getOptional (FirstSpec first_spec, RestSpecs ...rest_specs) {
    return getOptional(rest_specs...);
}

template <typename T, typename ...RestSpecs>
auto getOptional(Optional<T> optional, RestSpecs ...rest_specs) {
    return optional.value;
}

// ############################################################################
// Utility: Get Store
// ############################################################################
bool getStore () {
    return true;
}

template <typename FirstSpec, typename ...RestSpecs>
bool getStore (FirstSpec first_spec, RestSpecs ...rest_specs) {
    return getStore(rest_specs...);
}

template <typename ...RestSpecs>
bool getStore(Store store, RestSpecs ...rest_specs) {
    return store.value;
}

// ############################################################################
// Trait: Is Spec Contained In Specs
// ############################################################################
template <typename SearchType, typename ...Specs>
struct IsSpecContainedInSpecsImpl : std::false_type {};

template <typename SearchType, typename FirstSpec, typename ...RestSpecs>
struct IsSpecContainedInSpecsImpl<SearchType, FirstSpec, RestSpecs...> {
    static constexpr bool value = std::is_same_v<SearchType, std::remove_reference_t<FirstSpec>> || IsSpecContainedInSpecsImpl<SearchType, RestSpecs...>::value;
};

template <typename SearchType, typename ...Specs>
constexpr bool is_spec_contained_in_specs = IsSpecContainedInSpecsImpl<SearchType, Specs...>::value;

// ############################################################################
// Trait: Is Optional Contained In Specs
// ############################################################################
template <typename ...Specs>
struct IsOptionalContainedInSpecsImpl : std::false_type {};

template <typename FirstType, typename ...RestSpecs>
struct IsOptionalContainedInSpecsImpl<FirstType, RestSpecs...> {
    static constexpr bool value = IsOptionalContainedInSpecsImpl<RestSpecs...>::value;
};

template <typename T, typename ...RestSpecs>
struct IsOptionalContainedInSpecsImpl<Optional<T>, RestSpecs...> : std::true_type {
};

template <typename ...Specs>
constexpr bool is_optional_contained_in_specs = IsOptionalContainedInSpecsImpl<Specs...>::value;


// ############################################################################
// Trait & Concept: Is Optional
// ############################################################################
template <typename T>
struct IsOptionalImpl : std::false_type {};

template <typename T>
struct IsOptionalImpl<std::optional<T>> : std::true_type {};

template <typename T>
constexpr bool is_optional = IsOptionalImpl<T>::value;

template <typename T>
concept IsOptional = is_optional<T>;

// ############################################################################
// Trait & Concept: Is Valid Spec for Arg
// ############################################################################
template <typename>
struct IsValidSpecForArgImpl : std::false_type {};

template<>
struct IsValidSpecForArgImpl<Positional> : std::true_type {};

template <>
struct IsValidSpecForArgImpl<Mnemonic> : std::true_type {};

template <>
struct IsValidSpecForArgImpl<Help> : std::true_type {};

template <typename T>
struct IsValidSpecForArgImpl<Optional<T>> : std::true_type {};

template <typename T>
constexpr bool is_valid_spec_for_arg = IsValidSpecForArgImpl<T>::value;

template <typename T>
concept SpecForArg = is_valid_spec_for_arg<T>;

// ############################################################################
// Trait & Concept: Is Valid Spec for Optional Arg
// ############################################################################
template <typename>
struct IsValidSpecForOptionalArgImpl : std::false_type {};

template <>
struct IsValidSpecForOptionalArgImpl<Mnemonic> : std::true_type {};

template <>
struct IsValidSpecForOptionalArgImpl<Help> : std::true_type {};

template <typename T>
constexpr bool is_valid_spec_for_optional_arg = IsValidSpecForOptionalArgImpl<T>::value;

template <typename T>
concept SpecForOptionalArg = is_valid_spec_for_optional_arg<T>;

// ############################################################################
// Trait & Concept: Is Valid Spec for Switch Arg
// ############################################################################
template <typename>
struct IsValidSpecForSwitchArgImpl : std::false_type {};

template <>
struct IsValidSpecForSwitchArgImpl<Mnemonic> : std::true_type {};

template <>
struct IsValidSpecForSwitchArgImpl<Help> : std::true_type {};

template <>
struct IsValidSpecForSwitchArgImpl<Store> : std::true_type {};

template <typename T>
constexpr bool is_valid_spec_for_switch_arg = IsValidSpecForSwitchArgImpl<T>::value;

template <typename T>
concept SpecForSwitchArg = is_valid_spec_for_switch_arg<T>;

// ############################################################################
// Trait & Concept: Is Valid Spec for N-Ary Arg
// ############################################################################
template <typename>
struct IsValidSpecForNAryArgImpl : std::false_type {};

template <>
struct IsValidSpecForNAryArgImpl<Mnemonic> : std::true_type {};

template <>
struct IsValidSpecForNAryArgImpl<Help> : std::true_type {};

template <>
struct IsValidSpecForNAryArgImpl<Required> : std::true_type {};

template <>
struct IsValidSpecForNAryArgImpl<Positional> : std::true_type {};

template <typename T>
constexpr bool is_valid_spec_for_n_ary_arg = IsValidSpecForNAryArgImpl<T>::value;

template <typename T>
concept SpecForNAryArg = is_valid_spec_for_n_ary_arg<T>;

// ############################################################################
// Concept: Is Valid Type For Arg
// ############################################################################
template <typename T>
concept TypeForArg = IsConvertibleFromString<T>;

template <typename T>
concept TypeForOptionalArg = (
    TypeForArg<T> &&
    not std::is_same_v<T, bool>
);

template <typename T>
concept TypeForNAryArg = (
    TypeForArg<T> &&
    not std::is_same_v<T, bool>
);

// ############################################################################
// Class: Common Arg Data
// ############################################################################
struct ArgCommonData {
    std::string arg_name;
    std::optional<std::string> mnemonic;

    bool is_required {false};
    bool is_positional {false};
    bool is_n_ary {false};
    bool has_argument{false};
    
    std::optional<std::string> help;

    bool is_matched {false};
};

template <typename ...Specs>
auto getArgCommonData (const std::string& arg_name, Specs&& ...specs) {
    ArgCommonData result{};
    if constexpr (is_spec_contained_in_specs<Positional, Specs...>) {
        result.is_positional = true;
    }
    if constexpr (is_spec_contained_in_specs<Mnemonic, Specs...>) {
        result.mnemonic = getMnemonic(specs...);
    }
    if constexpr (is_spec_contained_in_specs<Help, Specs...>) {
        result.help = getHelp(specs...);
    }
    if constexpr (is_spec_contained_in_specs<Required, Specs...>) {
        result.is_required = true;
    }
    result.arg_name = arg_name;
    return result;
}

// ############################################################################
// Utilities: Arguments
// ############################################################################
inline bool isPositionalArgument (const std::string& arg) {
    return (not arg.starts_with("-"));
}

inline bool isArgumentMatched (const std::string& arg, const ArgCommonData& common_arg_data) {
    if (arg.size() > 2 and arg.starts_with("--")) {
        return arg.substr(2) == common_arg_data.arg_name;
    }
    else if (arg.size() > 1 and arg.starts_with("-")) {
        return common_arg_data.mnemonic.has_value() and arg.substr(1) == *common_arg_data.mnemonic;
    }
    return false;
}

// ############################################################################
// Trait: Construct Argument Data (TypeList)
// ############################################################################
template <typename ...>
struct ConstructArgumentDataImpl {
    using type = TypeList<>;
};

template <FixedString FirstArgName, typename FirstArgType, typename ...RestArgs>
struct ConstructArgumentDataImpl<ArgImpl<FirstArgName, FirstArgType>, RestArgs...> {
    using type = TypeListPrepend<
        Field<FirstArgName, FirstArgType>,
        typename ConstructArgumentDataImpl<RestArgs...>::type
    >;
};

template <typename ...Args>
using ConstructArgumentData = typename ConstructArgumentDataImpl<Args...>::type;

// ############################################################################
// Class: Argument Consumption
// ############################################################################
struct ArgConsumption {
    enum class Status {
        ERROR = -1,
        NO_MATCH = 0,
        MATCH = 1,
    };

    Status status;
    int consume_count{-1};
    std::string error_message{};
};

// ############################################################################
// Utilities: Argument
// ############################################################################
template <FixedString Name, typename Type>
class ArgImpl;

template <typename T>
ArgConsumption consumePositionalArgument (std::optional<T>& value, const std::string& arg) {
    auto converted_arg = convertFromString<T>(arg);
    if (converted_arg.has_value()) {
        value = *converted_arg;
        return {.status = ArgConsumption::Status::MATCH, .consume_count = 1};
    }
    else {
        return {.status = ArgConsumption::Status::ERROR, .error_message = "argument conversion failed"};
    }
}

template <typename T>
ArgConsumption consumeDashedArgument (std::optional<T>& value, const std::vector<std::string>& args, int pos) {
    if (pos + 1 >= args.size()) {
        return {.status = ArgConsumption::Status::ERROR, .error_message = "missing argument"};
    }
    auto converted_arg = convertFromString<T>(args[pos + 1]);
    if (converted_arg.has_value()) {
        value = *converted_arg;
        return {.status = ArgConsumption::Status::MATCH, .consume_count = 2};
    }
    else {
        return {.status = ArgConsumption::Status::ERROR, .error_message = "argument conversion failed"};
    }
}

struct StoreResult {
    bool success{false};
    std::string error_message{};
};

// ############################################################################
// Class: Argument
// ############################################################################
template <FixedString Name, TypeForArg Type>
class ArgImpl<Name, Type> {
public:
    template <SpecForArg ...Specs>
    requires (
        not (
            is_spec_contained_in_specs<Positional, Specs...> and
            is_optional_contained_in_specs<Specs...>
        )
    )
    ArgImpl (Specs&& ...specs) : common_data{getArgCommonData(Name.toString(), specs...)} {
        auto optional = getOptional(specs...);
        if constexpr (not std::is_same_v<decltype(optional), std::nullopt_t>) {
            value = convertFromOther<Type>(optional);
        }
        else {
            common_data.is_required = true;
        }
        common_data.has_argument = true;
    }

    ArgConsumption consume (const std::vector<std::string>& args, int pos) {
        if (common_data.is_positional and isPositionalArgument(args[pos])) {
            if (not common_data.is_matched) {
                common_data.is_matched = true;
                return consumePositionalArgument(value, args[pos]);
            }
        }
        else if (isArgumentMatched(args[pos], common_data)) {
            common_data.is_matched = true;
            auto result = consumeDashedArgument(value, args, pos);
            return result;
        }
        return {.status = ArgConsumption::Status::NO_MATCH};
    }
    
    template <typename NamedTuple>
    StoreResult storeValue (NamedTuple& nt) const {
        if (common_data.is_required and not value.has_value()) {
            return {.success = false, .error_message = "argument is required but not found: " + common_data.arg_name};
        }
        nt.template get<Name>() = *value;
        return {.success = true};
    }

    auto getValue () const {
        return value;
    }

    const ArgCommonData getCommonData() const {
        return common_data;
    }

private:
    ArgCommonData common_data{};
    std::optional<Type> value{};
};

// ############################################################################
// Class: Optional Argument
// ############################################################################
template <FixedString Name, TypeForOptionalArg Type>
class ArgImpl<Name, std::optional<Type>> {
public:
    template <SpecForOptionalArg ...Specs>
    ArgImpl (Specs&& ...specs) : common_data{getArgCommonData(Name.toString(), specs...)} {
        common_data.has_argument = true;
    }

    ArgConsumption consume (const std::vector<std::string>& args, int pos) {
        if (isArgumentMatched(args[pos], common_data)) {
            common_data.is_matched = true;
            return consumeDashedArgument(value, args, pos);
        }
        return {.status = ArgConsumption::Status::NO_MATCH};
    }

    template <typename NamedTuple>
    StoreResult storeValue (NamedTuple& nt) const {
        nt.template get<Name>() = value;
        return {.success = true};
    }

    auto getValue () const {
        return value;
    }

    const ArgCommonData getCommonData() const {
        return common_data;
    }

private:
    ArgCommonData common_data{};
    std::optional<Type> value{};
};

// ############################################################################
// Class: Switch Argument
// ############################################################################
template <FixedString Name>
class ArgImpl<Name, bool> {
public:
    template <SpecForSwitchArg ...Specs>
    ArgImpl (Specs&& ...specs) : common_data{getArgCommonData(Name.toString(), specs...)} {
        value = not getStore(specs...);
        common_data.has_argument = false;
    }

    ArgConsumption consume (const std::vector<std::string>& args, int pos) {
        if (isArgumentMatched(args[pos], common_data)) {
            if (not common_data.is_matched) {
                value = not value;
                common_data.is_matched = true;
            }
            return {.status = ArgConsumption::Status::MATCH, .consume_count = 1};
        }
        return {.status = ArgConsumption::Status::NO_MATCH};
    }

    template <typename NamedTuple>
    StoreResult storeValue (NamedTuple& nt) const {
        nt.template get<Name>() = value;
        return {.success = true};
    }

    bool getValue () const {
        return value;
    }

    const ArgCommonData getCommonData() const {
        return common_data;
    }

private:
    ArgCommonData common_data;
    bool value {};
};

// ############################################################################
// Class: N-Ary Argument
// ############################################################################
template <FixedString Name, TypeForNAryArg Type>
class ArgImpl<Name, std::vector<Type>> {
public:
    template <SpecForNAryArg ...Specs>
    ArgImpl (Specs&& ...specs) : common_data{getArgCommonData(Name.toString(), specs...)} {    
        common_data.is_n_ary = true;
        common_data.is_required = common_data.is_required || common_data.is_positional;
        common_data.has_argument = not common_data.is_positional;
    }

    ArgConsumption consume (const std::vector<std::string>& args, int pos) {
        if (common_data.is_positional and isPositionalArgument(args[pos])) {
            std::optional<Type> value;
            auto result = consumePositionalArgument(value, args[pos]);
            if (result.status == ArgConsumption::Status::MATCH and value.has_value()) {
                values.push_back(*value);
            }
            return result;
        }
        else if (isArgumentMatched(args[pos], common_data)) {
            std::optional<Type> value;
            auto result = consumeDashedArgument(value, args, pos);
            if (result.status == ArgConsumption::Status::MATCH and value.has_value()) {
                values.push_back(*value);
            }
            return result;
        }
        return {.status = ArgConsumption::Status::NO_MATCH};
    }

    template <typename NamedTuple>
    StoreResult storeValue (NamedTuple& nt) const {
        if (common_data.is_required and values.size() == 0) {
            return {.success = false, .error_message = "argument is required but not found: " + common_data.arg_name};
        }
        nt.template get<Name>() = values;
        return {.success = true};
    }

    const std::vector<Type> getValue () const {
        return values;
    }

    const ArgCommonData getCommonData() const {
        return common_data;
    }

private:
    ArgCommonData common_data;
    std::vector<Type> values;
};

// ############################################################################
// Utility: Common Arg Data
// ############################################################################
template <typename LastArg>
std::deque<ArgCommonData> collectArgCommonData (const LastArg& arg) {
    std::deque<ArgCommonData> result;
    result.push_front(arg.getCommonData());
    return result;
}

template <typename FirstArg, typename ...RestArgs>
std::deque<ArgCommonData> collectArgCommonData (const FirstArg& first_arg, const RestArgs& ...rest_args) {
    auto result = collectArgCommonData(rest_args...);
    result.push_front(first_arg.getCommonData());
    return result;
}

// ############################################################################
// Utility: Validate
// ############################################################################
struct ValidationResult {
    bool success {false};
    std::string error_message{};
};

bool isValidName (const std::string name) {
    for (const auto ch : name) {
        if (not ((ch >= 'a' and ch <= 'z') or (ch >= 'A' and ch <= 'Z') or (ch >= '0' and ch <= '9') or (ch == '_'))) {
            return false;
        }
    }
    return true;
}

ValidationResult validateArgumentNames (const std::deque<ArgCommonData>& common_data) {
    for (const auto& data : common_data) {
        if (not isValidName(data.arg_name)) {
            return {.success = false, .error_message = "argument name has invalid characters: " + data.arg_name};
        }
    }
    for (auto outer = 0LU; outer < common_data.size() - 1; ++outer) {
        for (auto inner = outer + 1; inner < common_data.size(); ++inner) {
            if (common_data[outer].arg_name == common_data[inner].arg_name) {
                return {.success = false, .error_message = "argument name is not unique: " + common_data[outer].arg_name};
            }
        }
    }
    return {.success = true};
}

ValidationResult validateMnemonics (const std::deque<ArgCommonData>& common_data) {
    for (const auto& data : common_data) {
        if (data.mnemonic.has_value() and not isValidName(*data.mnemonic)) {
            return {.success = false, .error_message = "argument mnemonic has invalid characters: " + *data.mnemonic};
        }
    }
    for (auto outer = 0LU; outer < common_data.size() - 1; ++outer) {
        for (auto inner = outer + 1; inner < common_data.size(); ++inner) {
            if (common_data[outer].mnemonic.has_value() and common_data[inner].mnemonic.has_value()) {
                if (*common_data[outer].mnemonic == *common_data[inner].mnemonic) {
                    return {.success = false, .error_message = "argument mnemonic is not unique: " + *common_data[outer].mnemonic};
                }
            }
        }
    }
    return {.success = true};
}

ValidationResult validateNAryArguments (const std::deque<ArgCommonData>& common_data) {
    bool positional_n_ary_found {false};
    std::string first_positional_n_ary_arg_name{};
    for (auto loop_index = 0LU; loop_index < common_data.size(); ++loop_index) {
        if (common_data[loop_index].is_positional and common_data[loop_index].is_n_ary) {
            if (not positional_n_ary_found) {
                first_positional_n_ary_arg_name = common_data[loop_index].arg_name;
            }
            else {
                return {.success = false, .error_message = "argument is never reached due to multiple n-ary positional arguments: " + common_data[loop_index].arg_name + "(shadowed by: " + first_positional_n_ary_arg_name + ")"}; 
            }
            positional_n_ary_found = true;
        }
    }
    return {.success = true};
}

template <typename ...Args>
ValidationResult validateArgs (const Args& ...args) {
    auto common_data = collectArgCommonData(args...);
    auto unique_arg_names_result = validateArgumentNames(common_data);
    if (not unique_arg_names_result.success) {
        return unique_arg_names_result;
    }

    auto unique_mnemonic_result = validateMnemonics(common_data);
    if (not unique_mnemonic_result.success) {
        return unique_mnemonic_result;
    }

    auto n_ary_result = validateNAryArguments(common_data);
    if (not n_ary_result.success) {
        return n_ary_result;
    }

    return {.success = true};
}

// ############################################################################
// Utility: Store Value
// ############################################################################
template <typename NamedTuple>
StoreResult storeValue (NamedTuple& nt) {
    return {.success = true};
}

template <typename NamedTuple, typename FirstArg, typename ...RestArgs>
StoreResult storeValue (NamedTuple& nt, const FirstArg& first_arg, const RestArgs& ...rest_args) {
    auto result = first_arg.storeValue(nt);
    if (not result.success) {
        return result;
    }
    return storeValue(nt, rest_args...);
}

// ############################################################################
// Utility: Consume Argument
// ############################################################################
template <typename LastArg>
ArgConsumption consumeArgument (const std::vector<std::string>& arguments, int pos, LastArg& last_arg) {
    auto result = last_arg.consume(arguments, pos);
    if (result.status == ArgConsumption::Status::ERROR) {
        result.error_message += " (context: arg-name = " + last_arg.getCommonData().arg_name + ", input argument = " + arguments[pos] + ")";
    }
    return result;
}

template <typename FirstArg, typename ...RestArgs>
ArgConsumption consumeArgument(const std::vector<std::string>& arguments, int pos, FirstArg& first_arg, RestArgs& ...rest_args) {
    auto result = first_arg.consume(arguments, pos);
    if (result.status == ArgConsumption::Status::MATCH) {
        return result;
    }
    else if (result.status == ArgConsumption::Status::ERROR) {
        result.error_message += " (context: arg-name = " + first_arg.getCommonData().arg_name + ", input argument = " + arguments[pos] + ")";
        return result;
    }
    return consumeArgument(arguments, pos, rest_args...);
}

// ############################################################################
// Utility: Help Printing
// ############################################################################

void printAttributes (std::ostream& os, const ArgCommonData& common_data) {
    if (common_data.is_required) {
        os << " [required]";
    }
    if (common_data.is_n_ary) {
        os << " [n-ary]";
    }
}

void printPositionalArgument(std::ostream& os, const ArgCommonData& common_data) {
    os << common_data.arg_name;
    if (common_data.is_n_ary) {
        os << "...";
    }
}

void printDashedArgument(std::ostream& os, const ArgCommonData& common_data) {
    std::string dashed_argument = "(--" + common_data.arg_name;
    if (common_data.has_argument) {
        dashed_argument += " ARG";
    }
    if (common_data.mnemonic.has_value()) {
        dashed_argument += "|-" + *common_data.mnemonic;
        if (common_data.has_argument) {
            dashed_argument += " ARG";
        }
    }
    dashed_argument += ")";
    if (common_data.is_n_ary) {
        dashed_argument += "...";
    }
    if (not common_data.is_required) {
        os << "[" << dashed_argument << "]";
    }
    else {
        os << dashed_argument;
    }
}


template <typename ArgumentTuple>
std::string composeHelpText (const std::string program_name, const std::string program_description, const ArgumentTuple& arg_description) {
    std::stringstream sstr;
    sstr << program_description << "\n";
    auto common_args = std::apply(
        [](const auto& ...args) {
            return internal::collectArgCommonData(args...);
        },
        arg_description
    );

    sstr << "\n";
    sstr << program_name;
    for (const auto& data : common_args) {
        sstr << " ";
        if (data.is_positional) {
            printPositionalArgument(sstr, data);  
        }
        else {
            printDashedArgument(sstr, data);
        }
    }
    sstr << "\n";
    
    sstr << "\n";
    sstr << "Positional\n";
    sstr << "----------\n";
    for (const auto& data : common_args) {
        if (data.is_positional) {
            sstr << std::string(2, ' ') << data.arg_name << ": " << (data.help.has_value() ? *data.help : "");
            printAttributes(sstr, data);
            sstr << "\n";
        }
    }

    sstr << "\n";
    sstr << "Arguments\n";
    sstr << "---------\n";
    for (const auto& data : common_args) {
        if (not data.is_positional) {
            sstr << std::string(2, ' ') << "--" << data.arg_name;
            if (data.mnemonic.has_value()) {
                sstr << "(-" << *data.mnemonic << ")";
            }
            if (data.has_argument) {
                sstr << " ARG";
            }
            sstr << ": " << (data.help.has_value() ? *data.help : "");
            printAttributes(sstr, data);
            sstr << "\n";
        }
    }
    return sstr.str();
}

} // namespace internal

} // namespace dacr

#endif // DATA_CRUNCHING_INTERNAL_ARGPARSE_HPP