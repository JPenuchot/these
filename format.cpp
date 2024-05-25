struct date {
  std::string_view year;
  std::string_view month;
  std::string_view day;
};

std::optional<date>
extract_date(std::string_view s) noexcept {
  using namespace ctre::literals;
  if (auto [whole, year, month, day] =
          ctre::match<
              "(\\d{4})/(\\d{1,2})/(\\d{1,2})">(
              s);
      whole) {
    return date{year, month, day};
  } else {
    return std::nullopt;
  }
}
